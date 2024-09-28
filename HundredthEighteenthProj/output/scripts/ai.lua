local objects = require "scripts/objects"
function nope()
end
function ai_enemy(self)
    if self:getState() == "attack" then
        self:setState("idle");
        local current = self:getGlobalCenter()
        local current_new = Vector2i.new(math.floor(current.x / setting.tile_size), 
                                       math.floor(current.y / setting.tile_size))
        local goal = self:getTarget()
        local goal_new = Vector2i.new(math.floor(goal.x / setting.tile_size), 
                                    math.floor(goal.y / setting.tile_size))
        --print("Current:",current_new.x, current_new.y)
        --print("Goal:", goal_new.x, goal_new.y)
        local path = map:getPath(current_new, goal_new)
        local next_cellur = path:back()
        --print("Current", current_new.x, current_new.y)
        --print("Next", next_cellur.x, next_cellur.y)
        local next_point = Vector2f.new(next_cellur.x * setting.tile_size + setting.tile_size / 2, 
                                    next_cellur.y * setting.tile_size + setting.tile_size / 2)
        local direction = next_point - current
        local normal = getNormalized(direction)
        if normal.x < 0 then
            self:mirrorFlip()
        else
            self:mirrorUnFlip()
        end
        --print("normal", normal.x, normal.y)
        self:setVelocity(normal * setting.speed_rate)
        self:playAnimation(setting.animation.walk, true)
    end
end