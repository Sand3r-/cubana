#pragma once
#include "types.h"

typedef union Event Event;

b32 ScriptEngineInit(void);
void ExecuteScript(const char* string);
void ExecuteScriptFile(const char* path);
void ScriptEngineProcessEvent(Event event);
b32 ScriptEngineShutdown(void);