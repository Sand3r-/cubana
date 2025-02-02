#pragma once
#include "file.h"
#include "memory/arena.h"

#define REPLAY_BUFFER_NAME_SIZE sizeof("PlaybackXX.cpf")
#define REPLAY_BUFFER_NUM 4
#define REPLAY_ARENAS_NUM 3

typedef struct ReplayBuffer
{
    char name[REPLAY_BUFFER_NAME_SIZE];
    File file;
} ReplayBuffer;

typedef struct ReplaySystemInitInfo
{
    Arena* arenas[REPLAY_ARENAS_NUM];
    ReplayBuffer* replay_buffers;
} ReplaySystemInitInfo;

void InitReplaySystem(ReplaySystemInitInfo* info);
void UpdateReplaySystem(void);