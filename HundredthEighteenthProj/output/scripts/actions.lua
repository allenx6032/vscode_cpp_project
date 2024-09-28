local objects = require "scripts/objects"
local PI = 3.1415926535
function nope()
end
function player_move_down(object)
    local velocity = object:getVelocity()
    object:setVelocity(velocity.x, setting.speed_rate)
    object:playAnimation(setting.animation.walk, true)
end
function player_move_left(object)
    local velocity = object:getVelocity()
    object:setVelocity(-setting.speed_rate, velocity.y)
    if object:playAnimation(setting.animation.walk, true) then
        object:mirrorFlip()
    end
end
function player_move_up(object)
    local velocity = object:getVelocity()
    object:setVelocity(velocity.x, -setting.speed_rate)
    object:playAnimation(setting.animation.walk, true)
end
function player_move_right(object)
    local velocity = object:getVelocity()
    object:setVelocity(setting.speed_rate, velocity.y)
    if object:playAnimation(setting.animation.walk, true) then
        object:mirrorUnFlip()
    end
end
function player_attack(object, mouse_pos)
    if object:getCurrentAnimationNum() ~= setting.animation.attack then
        object:playAnimation(setting.animation.attack)
        local projectile = oManager:newDynamicObject(setting.type.projectile1)
        local obj_pos = object:getPosition()
        local projectile_centre = projectile:getLocalCenter()
        projectile:setLooped(true)
        object:getVelocity()
        obj_pos = obj_pos + object:getVelocity() / 10.5
        mouse_pos = mouse_pos + object:getVelocity() / 10.5
        projectile:playAnimation(setting.animation.idle)
        if mouse_pos.x < object:getGlobalCenter().x then
            object:mirrorFlip()
        else
            object:mirrorUnFlip()
        end
        if object:isMirror() then
            projectile:setPosition(obj_pos.x + 160 - dynobjlist.player.projectile_pos[1] - projectile_centre.x, 
            obj_pos.y - projectile_centre.y + dynobjlist.player.projectile_pos[2])
        else
            projectile:setPosition(obj_pos.x + dynobjlist.player.projectile_pos[1] - projectile_centre.x, 
            obj_pos.y - projectile_centre.y + dynobjlist.player.projectile_pos[2])
        end
        projectile_centre = projectile:getGlobalCenter()
        local direction = Vector2f.new(mouse_pos.x - projectile_centre.x, mouse_pos.y - projectile_centre.y)
        local normal = getNormalized(direction);
        projectile:setVelocity(normal * setting.projectile_speed)
        if normal.x < 0 then
            projectile:mirrorFlip()
        else
            projectile:mirrorUnFlip()
        end
        local angle = atan((mouse_pos.y - projectile_centre.y) / (mouse_pos.x - projectile_centre.x)) * 180 / PI
        projectile:setRotationAroundCentre(angle)
        projectile:setSlowMode(false)
        projectile:setActivateTime(milliseconds(500))
        projectile:deactivate()
    end
end
function player_special(object, mouse_pos)
    local mob = oManager:newDynamicObject(setting.type.skeleton)
    mob:setCenter(mouse_pos)
end
