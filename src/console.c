#include "console.h"
#include <cimgui.h>
#include <cimgui_wrappers.h>
#include <stdbool.h>
#include <memory/linearallocator.h>
#include "culibc.h"
#include "error.h"

#define MAX_ITEMS 1024
#define MAX_ITEM_LENGTH 64

static struct
{
    char** items;
    u16    items_num;

    b32    auto_scroll;
    b32    scroll2bottom;
    ImGuiTextFilter text_filter;
} G;

int ConsoleInit()
{
    G.items = (char**)LinearMalloc(MAX_ITEMS * MAX_ITEM_LENGTH * sizeof(char));
    G.items_num = 0;

    return CU_SUCCESS;
}

static char* CreateNewString() // And return its address
{
    char* items = (char*)G.items;
    u32 current_index = G.items_num++;
    if (items + current_index > items + MAX_ITEM_LENGTH * MAX_ITEMS)
        ERROR("Out-of-str-memory. Please increase MAX_ITEMS");

    return items + current_index * MAX_ITEM_LENGTH;
}

static const char* GetStringAt(int index)
{
    char* items = (char*)G.items;
    if (index >= G.items_num)
        ERROR("Trying to access out-of-bound string.");
    return items + index * MAX_ITEM_LENGTH;
}

static void AddLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* dst = CreateNewString();
    cu_vsnprintf(dst, MAX_ITEM_LENGTH, fmt, args);
    dst[MAX_ITEM_LENGTH - 1] = 0;
    va_end(args);
}

static void ClearLog()
{
    G.items_num = 0;
}

void ConsoleUpdate()
{
    static bool is_console_open = true;
    igSetNextWindowSize(*ImVec2_ImVec2_Float(520, 600), ImGuiCond_FirstUseEver);
    if (!igBeginNoFlags("Console", &is_console_open))
    {
        igEnd();
        return;
    }

    if (igBeginPopupContextItemDef())
    {
        if (igMenuItem_BoolDef("Close Console"))
            is_console_open = false;
        igEndPopup();
    }

    igTextWrapped(
        "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
        "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    igTextWrapped("Enter 'HELP' for help.");

    if (igSmallButton("Add Debug Text"))
    {
        AddLog("%d some text", G.items_num); 
        AddLog("some more text"); 
        AddLog("display very important message here!");
    }
    igSameLineDef();

    if (igSmallButton("Add Debug Error"))
    {
        AddLog("[error] something went wrong");
    }
    igSameLineDef();

    if (igSmallButton("Clear"))
    {
        ClearLog();
    }
    igSameLineDef();

    bool copy_to_clipboard = igSmallButton("Copy");

    igSeparator();

    // Options menu
    if (igBeginPopup("Options", 0))
    {
        igCheckbox("Auto-scroll", &G.auto_scroll);
        igEndPopup();
    }

    // Options, Filter
    if (igButtonDef("Options"))
        igOpenPopup_StrDef("Options");
    igSameLineDef();
    ImGuiTextFilter_Draw(&G.text_filter, "Filter (\"incl,-excl\") (\"error\")", 180);
    igSeparator();

    // Reserve enough left-over height for 1 separator + 1 input text
    ImGuiStyle* style = igGetStyle();
    const float footer_height_to_reserve = style->ItemSpacing.y + igGetFrameHeightWithSpacing();
    ImVec2 scrolling_region_size = {0, -footer_height_to_reserve};
    if (igBeginChild_Str("ScrollingRegion", scrolling_region_size, false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (igBeginPopupContextWindow(NULL, 1))
        {
            ImVec2 empty = {0.0f, 0.0f};
            if (igSelectable_Bool("Clear", false, 0, empty))
                ClearLog();

            igEndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImVec2 item_spacing = {4, 1};
        igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, item_spacing); // Tighten spacing
        if (copy_to_clipboard)
            igLogToClipboard(-1);
        for (int i = 0; i < G.items_num; i++)
        {
            const char* item = GetStringAt(i);
            if (!ImGuiTextFilter_PassFilter(&G.text_filter, item, NULL)) 
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            // ImVec4 color;
            // bool has_color = false;
            // if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            // else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            // if (has_color)
            //     ImGui::PushStyleColor(ImGuiCol_Text, color);
            igTextUnformatted(item, NULL);
            // if (has_color)
            //     ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            igLogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (G.scroll2bottom || (G.auto_scroll && igGetScrollY() >= igGetScrollMaxY()))
            igSetScrollHereY(1.0f);
        G.scroll2bottom = false;

        igPopStyleVar(1);
    }
    igEndChild();
    igSeparator();

    igEnd();
}