#include "cimgui_wrappers.h"
#include "cimgui.h"

CIMGUI_API bool igBeginDef(const char* name)
{
    return igBegin(name, NULL, 0);
}

CIMGUI_API bool igBeginNoFlags(const char* name, bool* p_open)
{
    return igBegin(name, p_open, 0);
}

CIMGUI_API bool igBeginPopupContextItemDef()
{
    return igBeginPopupContextItem(NULL, 1);
}

CIMGUI_API bool igMenuItem_BoolDef(const char* label)
{
    return igMenuItem_Bool(label, NULL, false, true);
}

CIMGUI_API void igSameLineDef()
{
    igSameLine(0.0f, -1.0f);
}

CIMGUI_API bool igButtonDef(const char* label)
{
    ImVec2 size = { 0.0f, 0.0f };
    return igButton(label, size);
}

CIMGUI_API void igOpenPopup_StrDef(const char* str_id)
{
    igOpenPopup_Str(str_id, 0);
}
