-- imgui bindings
ig = {}
-- revealed, opened = ig.Begin(label, opened, window_flags)
ig.Begin = igBegin
-- ig.End()
ig.End = igEnd
-- clicked, checked = ig.Checkbox(text, checked)
ig.Checkbox = igCheckbox
-- clicked = ig.SmallButton(text)
ig.SmallButton = igSmallButton
-- button_pressed, input_text = ig.InputText(label, current_input_text)
ig.InputText = igInputText
-- was_value_changed, input_integer = ig.InputInt(label, shown_number)
ig.InputInt = igInputInt
-- was_value_changed, input_float = ig.InputFloat(label, shown_number)
ig.InputFloat = igInputFloat
-- ig.Text(fmt, ...)
ig.Text = function(fmt, ...)
    igText(string.format(fmt, ...))
end
