#pragma once
#include "types.h"

typedef union Event Event;

b32 ScriptEngineInit(void);
void ExecuteString(const char* string);
void ExecuteFile(const char* path);
void ScriptEngineProcessEvent(Event event);
b32 ScriptEngineShutdown(void);