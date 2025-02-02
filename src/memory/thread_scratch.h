#pragma once
#include "arena.h"
/*
    Per-thread scratch arena.
*/

int ThreadScratchArenaInitialise(void);
Arena* GetScratchArena(void);
void ThreadScratchArenaShutdown(void);