vec3 = require "vec3"
quat = require "quat"

function radians(degrees)
    return degrees * math.pi / 180
end

function init_camera_entity()
    local desc = {
        name = "Player's camera",
        position = vec3(0, 1, 2),
        rotation = quat.from_angle_axis(radians(-30), vec3(1, 0, 0)),
        flags = ENTITY_CAMERA_BIT,
    }
    return SpawnEntity(desc)
end

function init_player_entity()
    local desc = {
        name = "Player",
        position = vec3(0, 10, 0),
        dimensions = vec3(1, 1, 1),
        colour = vec3(0.64, 1, 1),
        flags = ENTITY_PLAYER_BIT | ENTITY_COLLIDES_BIT | ENTITY_GRAVITY_BIT | ENTITY_VISIBLE_BIT,
    }
    return SpawnEntity(desc) 
end

function init_player()
    local player = init_player_entity()
    local camera = init_camera_entity()
    SetWorldCamera(camera)
    player:AddChild(camera)
end

register("EVENT_GAME_BEGIN", init_player)