events = {}
callbacks = {}

-- Called from C
function _register_event(event_id, event_name)
    events[event_name] = event_id
    print("Registered event " .. event_name .. " of id " .. event_id)
end

function _handle_event(event_id)
    if callbacks[event_id] ~= nil then
        for index, value in ipairs(callbacks[event_id]) do
            value()
        end
    end
end

-- Called from lua scripts
function register(event_name, callback)
    event_id = events[event_name]
    if callbacks[event_id] ~= nil then
        table.insert(callbacks[event_id], callback)
    else
        callbacks[event_id] = { callback }
    end
end
