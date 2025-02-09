#include "replay.h"
#include "culibc.h"
#include "file.h"
#include "input.h"
#include "log/log.h"
#include "save.h"
#include <cimgui.h>
#include <assert.h>
#include <math.h>

static struct
{
    Arena* arenas[REPLAY_ARENAS_NUM];

    ReplayBuffer* replay_buffers;
    s8 recording_index;
    s8 playback_index;

    b8 playing;
    b8 looping;

    const Key bindings[4];
} C = {
    .bindings = { KEY_F5, KEY_F6, KEY_F7, KEY_F8 },
    .recording_index = -1,
    .playback_index = -1,
};

static void DrawReplayText(const char* text)
{
    // Blink
    if ((int)(fmod(igGetTime(), 1.0) * 2) == 0)
        return;

    ImVec4 colour = { 1, 0, 0, 1 };
    ImVec2 pos, size, dummy = {0};
    igGetWindowSize(&pos);
    pos.x = 0;
    pos.y = 2 * pos.y - 70.0f;
    igCalcTextSize(&size, text, NULL, false, -1);
    size.x += 10.0f; // Somehow its still too narrow
    igSetNextWindowPos(pos, 0, dummy);
    igSetNextWindowSize(size, 0);

    int flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoScrollbar;
    igBegin("Playback", NULL, flags);
    igTextColored(colour, text);
    igEnd();
}

static ReplayBuffer* GetReplayBuffer(s8 index)
{
    assert(index >= 0);
    assert(index < REPLAY_BUFFER_NUM);
    return &C.replay_buffers[index];
}

static void BeginRecordingInput(s8 recording_index)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(recording_index);
    C.recording_index = recording_index;
    C.playback_index = -1;

    char save_filename[32];
    cu_snprintf(save_filename, sizeof(save_filename), "Replay%02d.csf", recording_index + 1);

    // Save game state before input recording
    SaveGameState(save_filename);

    File file = FileOpen(replay_buffer->name, "wb");
    replay_buffer->file = file;

    L_INFO("Recording to %s", replay_buffer->name);
}

static void EndRecordingInput(s8 recording_index)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(recording_index);
    // Save file, disable recording
    FileClose(&replay_buffer->file);
    C.recording_index = -1;

    L_INFO("Finished recording to %s", replay_buffer->name);
}

static void RecordInput(s8 recording_index)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(recording_index);

    // Fetch Input
    Buffer buffer = GetInputState();
    // Save to file
    FileWrite(&replay_buffer->file, buffer.ptr, buffer.length, 1);

    DrawReplayText("¤ Recording");
}

static void BeginInputPlayback(s8 playback_index, b8 looping)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(playback_index);
    C.recording_index = -1;
    C.playback_index = playback_index;

    char save_filename[32];
    cu_snprintf(save_filename, sizeof(save_filename), "Replay%02d.csf", playback_index + 1);

    // Load game state before playing back input
    LoadGameState(save_filename);

    File file = FileOpen(replay_buffer->name, "rb");
    if (!file.valid)
    {
        C.playback_index = -1;
        L_ERROR("Error opening %s: %s", replay_buffer->name, file.error_msg);
        return;
    }

    replay_buffer->file = file;

    C.playing = true;
    if (!C.looping)
        L_INFO("Running playback from %s", replay_buffer->name);
    C.looping = looping;
}

static void PlaybackInput(s8 playback_index)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(playback_index);
    Buffer buffer = GetInputState();

    // Read input from file and overwrite it
    if (FileRead(&replay_buffer->file, buffer.ptr, buffer.length, 1) == 0)
    {
        // End playback
        C.playing = false;
    }

    DrawReplayText("» Playback");
}

static void EndInputPlayback(s8 playback_index)
{
    ReplayBuffer* replay_buffer = GetReplayBuffer(playback_index);
    FileClose(&replay_buffer->file);
    if (C.looping)
    {
        BeginInputPlayback(playback_index, true);
    }
    else
    {
        C.playing = false;
        C.playback_index = -1;
        ResetInputTo(KEY_STATE_UP); // Ctrl from recorded input may not be cleared

        L_INFO("Finished playback from %s", replay_buffer->name);
    }
}

void InitReplaySystem(ReplaySystemInitInfo* info)
{
    C.replay_buffers = info->replay_buffers;
    cu_memcpy(C.arenas, info->arenas, sizeof(Arena*) * REPLAY_ARENAS_NUM);

    for (s8 i = 0; i < REPLAY_BUFFER_NUM; i++)
    {
        cu_snprintf(C.replay_buffers[i].name, REPLAY_BUFFER_NAME_SIZE,
            "Playback%02d.cpf", i + 1);
    }
}

void UpdateReplaySystem()
{
    for (s8 i = 0; i < REPLAY_BUFFER_NUM; i++)
    {
        b8 ctrl = GetKeyState(KEY_CTRL) & KEY_STATE_DOWN;
        b8 alt = GetKeyState(KEY_ALT) & KEY_STATE_DOWN;
        b8 shift = GetKeyState(KEY_SHIFT) & KEY_STATE_DOWN;
        b8 fx = GetKeyState(C.bindings[i]) & KEY_STATE_PRESSED;

        if (!alt && ctrl && fx && !C.playing)
        {
            if (C.recording_index == -1)
                BeginRecordingInput(i);
            else if (C.recording_index == i)
                EndRecordingInput(i);
        }
        else if (!alt && fx && C.recording_index == -1)
        {
            if (C.playback_index == -1)
                BeginInputPlayback(i, !shift);
            else if (C.playback_index == i)
            {
                C.looping = false;
                EndInputPlayback(i);
            }
        }
    }

    if (C.recording_index != -1)
    {
        RecordInput(C.recording_index);
    }

    if (C.playback_index != -1)
    {
        PlaybackInput(C.playback_index);
        if (!C.playing)
        {
            EndInputPlayback(C.playback_index);
        }
    }
}