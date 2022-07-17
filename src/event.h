#pragma once
#include "types.h"

typedef enum EventType
{
    EVENT_TICK,
    EVENT_GAME_BEGIN,
    EVENT_GAME_END,
    
} EventType;

typedef struct EventTick
{
    EventType type; // = EVENT_TICK
    f32 delta;
} EventTick;

typedef struct EventGameBegin
{
    EventType type; // = EVENT_GAME_BEGIN
} EventGameBegin;

typedef struct EventGameEnd
{
    EventType type; // = EVENT_GAME_END
} EventGameEnd;

typedef union Event
{
    EventType      type;

    EventTick      event_tick;
    EventGameBegin event_game_begin;
    EventGameEnd   event_game_end;
} Event;

void EmitEvent(Event  event);
b32  PollEvent(Event* event);

Event CreateEventGameBegin(void);
Event CreateEventGameEnd(void);
Event CreateEventTick(f32 delta);