local BOOST = 300
local WIDTH = 1280
local HEIGHT = 720

dofile("Pipe.lua");

math.randomseed(os.time())



-- Set up the background layer
local background = newSprite(WIDTH, HEIGHT, 0, 0)
background:setTexture("res/bg.png");

local citybg = {
	newSprite(WIDTH, 256, 0, HEIGHT - 310),
	newSprite(WIDTH, 256, WIDTH, HEIGHT - 310)
}
for k, v in pairs(citybg) do
	v:setTexture("res/city2.png");
end

local city = {
	newSprite(WIDTH, 256, 0, HEIGHT - 320),
	newSprite(WIDTH, 256, WIDTH, HEIGHT - 320)
}
for k, v in pairs(city) do
	v:setTexture("res/city.png");
end

-- Set up the foregorund layer
local pipes = {
	Pipe:new(0),
	Pipe:new(1),
	Pipe:new(2),
	Pipe:new(3),
	Pipe:new(4),
	Pipe:new(5),
}

local grass = {
	newSprite(WIDTH, 64, 0, HEIGHT - 64),
	newSprite(WIDTH, 64, WIDTH, HEIGHT - 64)
}
for k, v in pairs(grass) do
  v:setTexture("res/grass.png");
end

-- Set up the player
local bird = {
	sprite = newSprite(64, 32, 100, 100),
	vx = 0,
	vy = 0
}

bird.sprite:setTexture("res/bird.png");

-- Parallax scrolling
local function parallax(table, speed, dt)
	for _, v in pairs(table) do
	  v:move(-speed * dt, 0)
	  local x, y = v:getPosition()
	  if x < -WIDTH then
		v:move(WIDTH * 2, 0)
	  end
	end
end

local STATE_BEGIN = 0
local STATE_GAME = 1
local STATE_GAME_OVER = 2

local state = 0
local score = 0

local function gameover()
	print("GAME OVER")
	print("Score: " .. score)
	print("Press space to reset...")
	state = STATE_GAME_OVER
end


local function startGame()
	print("Press space to begin...")

	bird.vx = 0
	bird.vy = 0
	bird.sprite:setPosition(100, 100)

	state = STATE_BEGIN
	score= 0
	i = 0
	for _, pipe in pairs(pipes) do
	  pipe:reset(i)
	  i = i + 1
	end

end
startGame()

local function stateBegin(dt)
	if Keyboard.isKeyPressed("Space") then
		state = STATE_GAME
		print("Score: " .. score)
	end
	parallax(grass, 200, dt)
	parallax(city, 100, dt)
	parallax(citybg, 50, dt)
end

local function stateGame(dt)
	if Keyboard.isKeyPressed("Space") and bird.vy > 0 then
		bird.vy = -BOOST * dt
	end

	local x, y = bird.sprite:getPosition()
	if y < 0 then
		bird.sprite:move(0, -y)
		bird.vy = 0
	elseif y > HEIGHT - 64 then
		gameover()
	end

	bird.vy = bird.vy + 10 * dt

	bird.sprite:move(bird.vx, bird.vy)
	bird.vx = bird.vx * 0.99 
	bird.vy = bird.vy * 0.99

	for _, pipe in pairs(pipes) do
	  pipe:update(dt)
	  if pipe:xPos() < x and not pipe.dirty then 
		pipe.dirty = true
		score = score + 1
		print("Score: " .. score)
	  end
	  if bird.sprite:intersects(pipe.top) or bird.sprite:intersects(pipe.bottom) then 
		gameover()
	  end
	end

	parallax(grass, 200, dt)
	parallax(city, 100, dt)
	parallax(citybg, 50, dt)
end

local function stateGameover(dt)
	local x, y = bird.sprite:getPosition()
	if y < HEIGHT - 64 then
		bird.vy = bird.vy + 5 * dt
	else 
		bird.vy = 0
		if Keyboard.isKeyPressed("Space") then
			startGame()
		end
	end

	bird.sprite:move(0, bird.vy)
	bird.vy = bird.vy * 0.99
end

-- Function called once per frame from C++ side
function update(dt)
	if state == STATE_BEGIN then
		stateBegin(dt)
	elseif state == STATE_GAME then 
		stateGame(dt)
	elseif state == STATE_GAME_OVER then
		stateGameover(dt)
	end
end