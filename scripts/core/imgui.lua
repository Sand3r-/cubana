-- imgui bindings

ImGui.Text = function(fmt, ...)
    if fmt ~= nil then
        ImGui.TextUnformatted(string.format(fmt, ...))
    end
end

ImGui.InputTextAuto = function(label, buffer)
    return ImGui.InputText(label, buffer, 1024)
end