vec3 = require "vec3"

function init_enemy()
    local desc = {
        name = "Enemy",
        position = vec3(3, 10, 0),
        dimensions = vec3(1, 1, 1),
        colour = vec3(1, 0, 0),
        flags = ENTITY_ENEMY_BIT | ENTITY_COLLIDES_BIT | ENTITY_GRAVITY_BIT | ENTITY_VISIBLE_BIT,
    }
    return SpawnEntity(desc)
end

function spawn_enemies(count)
    for i = 1, count do
        local desc = {
            name = "Enemy_" .. i,
            position = vec3(math.random(-10, 10), 10, math.random(-10, 10)), -- Random X/Z, Y=10
            dimensions = vec3(1, 1, 1),
            colour = vec3(1, 0, 0),
            flags = ENTITY_ENEMY_BIT | ENTITY_COLLIDES_BIT | ENTITY_GRAVITY_BIT | ENTITY_VISIBLE_BIT,
        }
        SpawnEntity(desc)
    end
end

function init_enemies()
    spawn_enemies(10)
end

spawn_timer = 0

function timed_spawn_enemy(event)
    spawn_timer = spawn_timer + event.dt
    if spawn_timer >= 4000 then
        spawn_timer = spawn_timer - 4000
        spawn_enemies(1)
        log.info("Spawned a new enemy")
    end
end

register("EVENT_GAME_BEGIN", init_enemy)
register("EVENT_GAME_BEGIN", init_enemies)
-- register("EVENT_TICK", timed_spawn_enemy)