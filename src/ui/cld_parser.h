#pragma once
// Lua bindings documentation
//
// Contains data needed to link between a function name and an appropriate
// file and corresponding line number (or numbers if the function is
// overloaded.)
typedef struct LuaBindDoc
{
    // Name of the referenced source file
    const char* source;
    // Function names
    const char** names;
    // Per-function array of line numbers of all overloads, first entry is
    // the table size
    int** lines;
    // Number of functions
    int num;
} LuaBindDoc;

typedef struct Arena Arena;

// Parse the file and populate LuaBindDoc struct
LuaBindDoc* LoadLuaDoc(Arena* arena, const char* filename);