#pragma once
#include "file.h"
#include "memory/arena.h"

#define SAVE_ARENAS_NUM 3

typedef struct SaveSystemInitInfo
{
    Arena* arenas[SAVE_ARENAS_NUM];
} SaveSystemInitInfo;

void InitSaveSystem(SaveSystemInitInfo* info);
b8 SaveGameState(const char* filename);
b8 LoadGameState(const char* filename);
void UpdateSaveSystem(void);