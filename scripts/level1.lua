function init_world()
    print("And so the game begun from the lua point of view.")
end

function tick_test()
    print("Test.")
end

register("EVENT_GAME_BEGIN", init_world)
-- register("EVENT_TICK", tick_test)