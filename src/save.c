#include "save.h"
#include "culibc.h"
#include "file.h"
#include "input.h"
#include "log/log.h"
#include <assert.h>

static struct
{
    Arena* arenas[SAVE_ARENAS_NUM];
} C;

void InitSaveSystem(SaveSystemInitInfo* info)
{
    cu_memcpy(C.arenas, info->arenas, sizeof(Arena*) * SAVE_ARENAS_NUM);
}

b8 SaveGameState(const char* filename)
{
    File file = FileOpen(filename, "wb");
    if (!file.valid)
    {
        L_ERROR("Failed to open %s for saving", filename);
        return false;
    }

    for (u8 i = 0; i < SAVE_ARENAS_NUM; i++)
    {
        Arena* arena = C.arenas[i];
        FileWrite(&file, arena, sizeof(Arena), 1);
        FileWrite(&file, arena->base, arena->size, 1);
    }

    FileClose(&file);
    L_INFO("Game state saved to %s", filename);
    return true;
}

b8 LoadGameState(const char* filename)
{
    File file = FileOpen(filename, "rb");
    if (!file.valid)
    {
        L_ERROR("Failed to open %s for loading", filename);
        return false;
    }

    for (u8 i = 0; i < SAVE_ARENAS_NUM; i++)
    {
        Arena* arena = C.arenas[i];
        Arena serialised_arena = {0};
        FileRead(&file, &serialised_arena, sizeof(Arena), 1);

        if (serialised_arena.size > arena->size)
            ArenaResize(arena, serialised_arena.size);
        cu_memcpy(arena, &serialised_arena, sizeof(Arena));
        FileRead(&file, arena->base, arena->size, 1);
    }

    FileClose(&file);
    L_INFO("Game state loaded from %s", filename);
    return true;
}

void UpdateSaveSystem(void)
{
    b8 save_pressed = GetKeyState(KEY_F10) & KEY_STATE_PRESSED;
    if (save_pressed)
    {
        SaveGameState("quicksave.cpf");
    }

    b8 load_pressed = GetKeyState(KEY_F12) & KEY_STATE_PRESSED;
    if (load_pressed)
    {
        LoadGameState("quicksave.cpf");
    }
}