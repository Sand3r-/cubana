require "log"

events = {}
callbacks = {}

-- Called from C
function _register_event(event_id, event_name)
    events[event_name] = event_id
    log.info("Registered event " .. event_name .. " of id " .. event_id)
end

function _handle_event(event)
    if callbacks[event.type] ~= nil then
        for index, value in ipairs(callbacks[event.type]) do
            value(event)
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
