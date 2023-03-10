require "log"
require "imgui"
require "im3d"
vec3 = require "vec3"
mat4 = require "mat4"

function print_available_symbols()
    for n in pairs(_G) do print(n) end
end

function init_world(event)
    log.info("And so the game begun from the lua point of view.")
end

function tern(condition, true_, false_)
    return condition and true_ or false_
end

function bool2str(bool)
    return tern(bool, "true", "false")
end

g_im3d_gizmo_matrix = mat4.identity(mat4.new())
g_imgui_menu_opened = true
g_imgui_checkbox = false
g_imgui_switch = false
g_imgui_text_buffer = ""
g_imgui_input_integer = 0
g_imgui_input_float = 0.0

auto_scroll = true
scroll_to_bottom = false
copy_to_clipboard = false
g_console_open = true
items = {}

function imgui_demo(dt)
    if g_imgui_menu_opened then
        revealed, g_imgui_menu_opened = imgui.Begin("Demo menu", g_imgui_menu_opened, 0)
        clicked, g_imgui_checkbox = imgui.Checkbox("Checkbox", g_imgui_checkbox)
        imgui.Text("Delta time: %f", dt)
        imgui.Text("Checked?: %s", bool2str(g_imgui_checkbox))
        if imgui.SmallButton("Flip the switch") then
            g_imgui_switch = not g_imgui_switch
        end
        button_pressed, new_buffer = imgui.InputTextAuto("Test", g_imgui_text_buffer)
        if button_pressed then
            g_imgui_text_buffer = new_buffer
        end

        _, g_imgui_input_integer = imgui.InputInt("Value", g_imgui_input_integer)
        imgui.Text("integer = %d ", g_imgui_input_integer)

        _, g_imgui_input_float = imgui.InputFloat("ValueFloat", g_imgui_input_float)
        imgui.Text("float = %f ", g_imgui_input_float)

        imgui.Text(g_imgui_text_buffer)
        imgui.Text("Is input text active? %s", bool2str(button_pressed))
        imgui.Text("Switch?: %s", bool2str(g_imgui_switch))
        imgui.End()
    end
end

function im3d_demo()
    x = vec3.new(0, 0, 0)
    y = vec3.new(5, 5, 5)

    if g_imgui_checkbox then
        im3d.DrawLine(vec3.new(0, 0, 1), vec3.new(3, 3, 1), 15, im3d.Color_Orange)
        im3d.DrawLine(x, y, 15, im3d.Color_Teal)

        is_changed, g_im3d_gizmo_matrix = im3d.Gizmo("Patka", g_im3d_gizmo_matrix)
        im3d.PushMatrix(g_im3d_gizmo_matrix)
        im3d.DrawSphere(vec3.new(0, 0, 0), 2.0)
        im3d.PopMatrix()
    end
end

function AddLog(fmt, ...)
    text = string.format(fmt, ...)
    table.insert(items, text)
end

function ClearLog()
    items = {}
end

function ExecCommand(command)
    AddLog(command)
end

function imgui_console(event_dt)
    imgui.SetNextWindowSize(520, 600, imgui.constant.Cond.FirstUseEver)
    _, g_console_open = imgui.Begin("Console", g_console_open)
    if not g_console_open then
        imgui.End()
    end

    if imgui.BeginPopupContextItem() then
        if imgui.MenuItem("Close Console") then
            g_console_open = false
        end
    end
    
    imgui.TextWrapped("Enter 'HELP' for help.");

    if imgui.SmallButton("Add Debug Text") then
        AddLog("%d some text", #items)
        AddLog("some more text")
        AddLog("display very important message here!");
    end
    imgui.SameLine()
    if imgui.SmallButton("Add Debug Error") then
        AddLog("[error] something went wrong")
    end
    imgui.SameLine()
    if imgui.SmallButton("Clear") then
        ClearLog()
        AddLog("Nothing xD")
    end
    imgui.SameLine();
    copy_to_clipboard = imgui.SmallButton("Copy")

    imgui.Separator();

    -- Options menu
    if imgui.BeginPopup("Options") then
        pressed, auto_scroll = imgui.Checkbox("Auto-scroll", auto_scroll)
        imgui.EndPopup()
    end

    -- Options, Filter
    if imgui.Button("Options") then
        imgui.OpenPopup("Options")
    end
    -- imgui.SameLine()
    -- Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    imgui.Separator()

    footer_height_to_reserve = imgui.GetFrameHeightWithSpacing();
    -- footer_height_to_reserve = imgui.GetStyle().ItemSpacing.y + imgui.GetFrameHeightWithSpacing();
    if imgui.BeginChild("ScrollingRegion", 0, -footer_height_to_reserve, false, imgui.constant.WindowFlags.HorizontalScrollbar) then
        if imgui.BeginPopupContextWindow() then
            if imgui.Selectable("Clear") then
                ClearLog()
            end
            imgui.EndPopup()
        end

        -- // Display every line as a separate entry so we can change their color or add custom widgets.
        -- // If you only want raw text you can use imgui.TextUnformatted(log.begin(), log.end());
        -- // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        -- // to only process visible items. The clipper will automatically measure the height of your first item and then
        -- // "seek" to display only items in the visible area.
        -- // To use the clipper we can replace your standard loop:
        -- //      for (int i = 0; i < Items.Size; i++)
        -- //   With:
        -- //      ImGuiListClipper clipper;
        -- //      clipper.Begin(Items.Size);
        -- //      while (clipper.Step())
        -- //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        -- // - That your items are evenly spaced (same height)
        -- // - That you have cheap random access to your elements (you can access them given their index,
        -- //   without processing all the ones before)
        -- // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        -- // We would need random-access on the post-filtered list.
        -- // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        -- // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        -- // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        -- // to improve this example code!
        -- // If your items are of variable height:
        -- // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        -- // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        imgui.PushStyleVar_2(imgui.constant.StyleVar.ItemSpacing, 4.0, 1.0) -- // Tighten spacing
        if copy_to_clipboard then
            imgui.LogToClipboard()
        end
        for i=1,#items do
            item = items[i];
            -- if (!Filter.PassFilter(item))
            --     continue;

            -- // Normally you would store more information in your item than just a string.
            -- // (e.g. make Items[] an array of structure, store color/type etc.)
            color = vec3.new(0.0, 0.0, 0.0)
            has_color = false;
            if string.find(item, "[error]", 1, true) then
                color = vec3.new(1.0, 0.4, 0.4)
                has_color = true
            elseif string.find(item, "# ", 1, true) then
                color = vec3.new(1.0, 0.8, 0.6)
                has_color = true
            end

            if has_color then
                imgui.PushStyleColor_2(imgui.constant.Col.Text, color.x, color.y, color.z, 1.0)
            end
            imgui.TextUnformatted(item);
            if has_color then
                imgui.PopStyleColor()
            end
        end
        if copy_to_clipboard then
            imgui.LogFinish()
        end

        -- // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        -- // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if scroll_to_bottom or (auto_scroll and imgui.GetScrollY() >= imgui.GetScrollMaxY()) then
            imgui.SetScrollHereY(1.0);
        end
        scroll_to_bottom = false;

        imgui.PopStyleVar()
    end
    imgui.EndChild()
    imgui.Separator()

    -- // Command-line
    input_buffer = ""
    reclaim_focus = false
    input_text_flags = imgui.constant.InputTextFlags.EnterReturnsTrue -- | imgui.constant.InputTextFlags.CallbackCompletion  | imgui.constant.InputTextFlags.CallbackHistory
    changed, input_buffer = imgui.InputText("Input", input_buffer, imgui.constant.MAX_TEXT_BUFFER_SIZE, input_text_flags, nil, nil)
    if changed then
        ExecCommand(input_buffer);
        input_buffer = ""
        reclaim_focus = true;
    end

    -- // Auto-focus on window apparition
    imgui.SetItemDefaultFocus();
    if reclaim_focus then
        imgui.SetKeyboardFocusHere(-1); -- // Auto focus previous widget
    end

    imgui.End()
end

function tick_test(event)
    imgui_demo(event.dt)
    imgui_console(event.dt)
    im3d_demo()
end

register("EVENT_GAME_BEGIN", init_world)
register("EVENT_TICK", tick_test)