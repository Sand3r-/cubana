#pragma once
#include "cimgui.h"

CIMGUI_API bool igBeginDef(const char* name);
CIMGUI_API bool igBeginNoFlags(const char* name, bool* p_open);
CIMGUI_API bool igBeginPopupContextItemDef();
CIMGUI_API bool igMenuItem_BoolDef(const char* label);
CIMGUI_API void igSameLineDef();
CIMGUI_API bool igButtonDef(const char* label);
CIMGUI_API void igOpenPopup_StrDef(const char* str_id);
