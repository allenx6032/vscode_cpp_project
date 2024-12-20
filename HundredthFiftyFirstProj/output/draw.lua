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
	
	sdl.SetRenderDrawColor(renderer, 0,255,0,0)
	
	sdl.Triangle(renderer, 30,30, 60,170, 120,130);
	sdl.FillTriangle(renderer, 30,230, 60,370, 120,330);
	
	sdl.Circle(renderer, 300,100, 40);
	sdl.FillCircle(renderer, 300,300, 40);
	
	sdl.Ellipse(renderer, 500, 100, 70, 30);
	sdl.FillEllipse(renderer, 500, 300, 70, 30);
	
	sdl.RenderPresent(renderer)
	
end