#pragma once
#include "math/vec.h"
#include "types.h"
#include "memory/arena.h"

typedef struct ObjectData
{
    char* name;
    v3 position;
    v3 dimensions;
    v3 colour;
} ObjectData;

typedef struct LevelData
{
    u16 count;
    ObjectData* objects;

} LevelData;

LevelData* LoadTextLevelData(Arena* arena, const char* filename);