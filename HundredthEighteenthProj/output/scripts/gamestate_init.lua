--gamestate_init_lua
local objects = require "scripts/objects"
local assets = require "scripts/assets"
--TEXTURE INITIALIZATION
for key1, val1 in pairs(textures) do
	local texture = asManager:createTexture(textures[key1].name)
	assert(texture)
	assert(texture:loadFromFile(textures[key1].path, IntRect.new()), "_____ERROR: Texture is no loaded")
end
--ANIMATION INITIALIZATION
for key1, val in pairs(animations) do
	local animation = asManager:createAnimation(animations[key1].name)
	local texture = asManager:getTexture(animations[key1].texture)
	animation:setSpriteSheet(texture);
	for _, frame in pairs(animations[key1].frames) do
		animation:addFrame(IntRect.new(frame[1], frame[2], frame[3], frame[4]))
	end
end
--DYNAMIC OBJECTS INITIALIZATION
for key1, val1 in pairs(dynobjlist) do
	local obj = oManager:newDynamicObjectType(dynobjlist[key1].type)
	assert(obj)
	for i, vertex in ipairs(dynobjlist[key1].boundybox) do
		obj:setBoundyBoxVertex(vertex[1], vertex[2], i - 1)
	end
	obj:setInteractRadius(dynobjlist[key1].interact_radius)
	obj:setHp(dynobjlist[key1].hp)
	obj:setMaxHp(dynobjlist[key1].hp)
	obj:setDamage(dynobjlist[key1].damage)
	obj:setAIclass(dynobjlist[key1].ai_class)
	obj:setScale(dynobjlist[key1].scale[1], dynobjlist[key1].scale[2])
	for key2, val2 in pairs(dynobjlist[key1].animation) do
		local animation = asManager:getAnimation(val2)
		assert(animation)
		obj:addAnimation(animation, setting.animation[key2])
	end
	obj:setAnimation(setting.animation.idle)
end
--STATIC OBJECTS INITIALIZATION
for key1, val1 in pairs(stobjlist) do
	local obj = oManager:newStaticObjectType(stobjlist[key1].type)
	assert(obj)
	for i, vertex in ipairs(stobjlist[key1].boundybox) do
		obj:setBoundyBoxVertex(vertex[1], vertex[2], i - 1)
	end
	obj:setScale(stobjlist[key1].scale[1], stobjlist[key1].scale[2])
	for key2, val2 in pairs(stobjlist[key1].animation) do
		local animation = asManager:getAnimation(val2)
		assert(animation)
		obj:addAnimation(animation, setting.animation[key2])
		obj:setAnimation(setting.animation[key2])
	end
end
--SPRITE OBJECTS INITIALIZATION
for key1, val1 in pairs(spritelist) do
	local obj = oManager:newSpriteObjectType(spritelist[key1].type)
	assert(obj)
	local texture = asManager:getTexture(spritelist[key1].texture)
	assert(texture)
	obj:setTexture(texture)
	obj.sprite:setTextureRect(IntRect.new(spritelist[key1].rect[1], spritelist[key1].rect[2], spritelist[key1].rect[3], spritelist[key1].rect[4]))
	obj.sprite:setScale(spritelist[key1].scale[1], spritelist[key1].scale[2])
end
local mainObj = oManager:newDynamicObject(setting.type.player)
mainObj:setPosition(0, 0)
player:setObject(mainObj)
--local ball2 = oManager:newDynamicObject(setting.type.ball)
--ball2:setPosition(550, 100)
--ball2:setRotation(45)
--local wall2 = oManager:newStaticObject(setting.type.wall)
--wall2:setPosition(1000, 1032)