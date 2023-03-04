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

function tick_test(event)
    imgui_demo(event.dt)
    im3d_demo()
end

register("EVENT_GAME_BEGIN", init_world)
register("EVENT_TICK", tick_test)