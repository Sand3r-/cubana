#include "ui.h"
#include "scripteditor.h"

void InitUI(Arena* arena)
{
    InitializeScriptEditor(arena);
}

void UpdateUI()
{
    UpdateScriptEditor();
}
