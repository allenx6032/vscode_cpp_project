local sdl = require("libsdluna")
local win = sdl.CreateWindow("hello", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED, 210, 210, sdl.WINDOW_SHOWN);
local ren = sdl.CreateRenderer(win, -1, 0);

while true do
	local event = sdl.PollEvent()
	if event ~= nil and event['type'] == sdl.EVENT_QUIT then
		break
	end
	-- Clear
	sdl.SetRenderDrawColor(ren, 0,0,0,255)
	sdl.RenderClear(ren)
	
	-- Draw
	sdl.SetRenderDrawColor(ren, 255,0,0,255)
	sdl.FillRect(ren, {0,0,70,70})
	
	sdl.SetRenderDrawColor(ren, 0,255,0,255)
	sdl.FillRect(ren, {0,70,70,70})
	
	sdl.SetRenderDrawColor(ren, 0,0,255,255)
	sdl.FillRect(ren, {0,140,70,70})
	
	sdl.SetRenderDrawColor(ren, 255,255,0,255)
	sdl.FillRect(ren, {70,0,70,70})
	
	sdl.SetRenderDrawColor(ren, 0,255,255,255)
	sdl.FillRect(ren, {70,70,70,70})
	
	sdl.SetRenderDrawColor(ren, 255,0,255,255)
	sdl.FillRect(ren, {70,140,70,70})
	
	sdl.SetRenderDrawColor(ren, 0,0,0,255)
	sdl.FillRect(ren, {140,0,70,70})
	
	sdl.SetRenderDrawColor(ren, 127,127,127,255)
	sdl.FillRect(ren, {140,70,70,70})
	
	sdl.SetRenderDrawColor(ren, 255,255,255,255)
	sdl.FillRect(ren, {140,140,70,70})
	
	-- Display
	sdl.RenderPresent(ren)
end
