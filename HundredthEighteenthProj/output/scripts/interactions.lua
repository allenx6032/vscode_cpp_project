local objects = require "scripts/objects"
function nope()
end
function player_skeleton_king(self, obj, mtv, mode)
    if mode == setting.collide_mode.convex_mode then
        self:move(mtv)
    end
end
function wall_block(self, obj, mtv, mode)
    self:move(mtv)
end
function projectile_delete(self, obj, mtv, mode)
    self:setVelocity(0, 0)
    self:playAnimation(setting.animation.death)
    self:setDeactivateTime(milliseconds(780))
    self:clockRestart()
    self:move(mtv)
    self:setGhostMode(true)
end
function skeleton_king_player(self, obj, mtv, mode)
    if mode == setting.collide_mode.circle_mode and self:getState() ~= "death" then
        self:setState("attack")
        self:setTarget(obj:getGlobalCenter())
    end
end
function damage(self, obj, mtv, mode)
    self:setHp(self:getHp() - obj:getDamage())
    if self:getHp() <= 0 then
        self:setLooped(false)
        self:setVelocity(0, 0)
        self:move(mtv)
        self:playAnimation(setting.animation.death)
        self:setDeactivateTime(seconds(10))
        self:clockRestart()
        self:setState("death")
    end
end