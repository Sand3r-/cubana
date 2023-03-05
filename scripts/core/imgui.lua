-- imgui bindings

imgui.Text = function(fmt, ...)
    imgui.TextUnformatted(string.format(fmt, ...))
end

imgui.InputTextAuto = function(label, buffer)
    return imgui.InputText(label, buffer, imgui.constant.MAX_TEXT_BUFFER_SIZE)
end