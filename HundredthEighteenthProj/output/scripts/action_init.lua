local objects = require "scripts/objects"
oManager:addScript("player_move_down", setting.type.player, setting.action.move_down)
oManager:addScript("player_move_left", setting.type.player, setting.action.move_left)
oManager:addScript("player_move_up", setting.type.player, setting.action.move_up)
oManager:addScript("player_move_right", setting.type.player, setting.action.move_right)
oManager:addScript("player_attack", setting.type.player, setting.action.attack)
oManager:addScript("player_special", setting.type.player, setting.action.special)