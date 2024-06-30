#include "imfont_storage.h"
#include <cimgui.h>

static struct FontStorage
{
    ImFont* editor_font;
} Storage;

void InitialiseImFontStorage(void)
{
       
    ImGuiIO* io = igGetIO();
    Storage.editor_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/FiraCode-Regular.ttf", 20, NULL, NULL);
}

ImFont* FontStorage_GetEditorFont()
{
    return Storage.editor_font;
}
