#pragma once
#include "arena.h"
/*
    Per-thread scratch arena.
*/

int ThreadScratchArenaInitialise();
Arena* GetScratchArena();
void ThreadScratchArenaShutdown();