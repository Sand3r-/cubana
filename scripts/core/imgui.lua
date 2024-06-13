-- imgui bindings

imgui.Text = function(fmt, ...)
    if fmt ~= nil then
        imgui.TextUnformatted(string.format(fmt, ...))
    end
end

imgui.InputTextAuto = function(label, buffer)
    return imgui.InputText(label, buffer, 1024)
end