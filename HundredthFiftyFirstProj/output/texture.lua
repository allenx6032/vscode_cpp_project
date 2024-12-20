local sdl = require("libsdluna")
local window = sdl.CreateWindow("SDLuna", 
								sdl.WINDOWPOS_UNDEFINED, 
								sdl.WINDOWPOS_UNDEFINED,
								200, 200,
								sdl.WINDOW_SHOWN)
local renderer = sdl.CreateRenderer(window, -1, sdl.RENDERER_ACCELERATED)
local texture = sdl.LoadImage(renderer, "logo.png")
sdl.SetRenderTarget(renderer, texture)
print(sdl.GetError())
while true do
	local event = sdl.PollEvent()
	if event ~= nil and event.type == sdl.EVENT_QUIT then
		break
	end
	
	sdl.SetRenderDrawColor(renderer, 0xff,0xff,0xff,0)
	sdl.RenderClear(renderer)
	sdl.RenderCopy(renderer, texture)
	sdl.RenderPresent(renderer)
end
