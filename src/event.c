#include "event.h"
#include <assert.h>
#include "lua.h"

#define MAX_EVENTS_NUM 256

static struct
{
    Event events[MAX_EVENTS_NUM];
    u32   events_num;
} E = {
    .events_num = 0
};

void EmitEvent(Event  event)
{
    assert(E.events_num < MAX_EVENTS_NUM);
    E.events[E.events_num++] = event;
}

b32 PollEvent(Event* event)
{
    if (E.events_num)
    {
        *event = E.events[--E.events_num];
        return true;
    }
    return false;
}

Event CreateEventGameBegin(void)
{
    Event event = {
        .event_game_begin = {
            .type = EVENT_GAME_BEGIN
        }
    };
    return event;
}

Event CreateEventGameEnd(void)
{
    Event event = {
        .event_game_end = {
            .type = EVENT_GAME_END
        }
    };
    return event;
}

Event CreateEventTick(f32 delta)
{
    Event event = {
        .event_tick = {
            .type = EVENT_TICK,
            .delta = delta
        }
    };
    return event;
}

