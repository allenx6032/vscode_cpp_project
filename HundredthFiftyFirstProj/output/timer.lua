local sdl = require("libsdluna")
local window = sdl.CreateWindow("Timer", 
								sdl.WINDOWPOS_UNDEFINED,
								sdl.WINDOWPOS_UNDEFINED,
								600, 400, 
								sdl.WINDOW_SHOWN)
local renderer = sdl.CreateRenderer(window, -1, sdl.RENDERER_SOFTWARE)

math.randomseed(os.time())
local counter1 = sdl.CreateCounter()
local counter2 = sdl.CreateCounter()
local counter3 = sdl.CreateCounter()
local rect1 = {-100, 16, 100, 100}
local rect2 = {-100, 132, 100, 100}
local rect3 = {-100, 248, 100, 100}
while true do
	local event = sdl.PollEvent()
	if event ~= nil and event.type == sdl.EVENT_QUIT then
		break
	end
	
	if counter1:time(10) then
		rect1[1] = rect1[1] + 1
		if rect1[1] >= 600 then
			rect1[1] = -100
		end
	end
	
	if counter2:time(50) then
		rect2[1] = rect2[1] + 1
		if rect2[1] >= 600 then
			rect2[1] = -100
		end
	end
	
	if counter3:time(100) then
		rect3[1] = rect3[1] + 1
		if rect2[1] >= 600 then
			rect2[1] = -100
		end
	end

	sdl.SetRenderDrawColor(renderer, 0,0,0,0)
	sdl.RenderClear(renderer)
	sdl.SetRenderDrawColor(renderer, 255,0,0,0)
	sdl.FillRect(renderer, rect1)
	sdl.SetRenderDrawColor(renderer, 0,255,0,0)
	sdl.FillRect(renderer, rect2)
	sdl.SetRenderDrawColor(renderer, 0,0,255,0)
	sdl.FillRect(renderer, rect3)
	sdl.RenderPresent(renderer)
	
end