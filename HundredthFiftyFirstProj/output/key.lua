local sdl = require("libsdluna")
local win = sdl.CreateWindow("hello", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED, 210, 210, sdl.WINDOW_SHOWN);
local ren = sdl.CreateRenderer(win, -1, 0);

while true do
	event = sdl.PollEvent() 
	if event == nil then
		
	elseif event['type'] == sdl.EVENT_QUIT then
		break
	elseif event['type'] == sdl.EVENT_KEYDOWN then
		print(event['key'])
	end

	-- Clear
	sdl.SetRenderDrawColor(ren, 0,0,0,255)
	sdl.RenderClear(ren)
	-- Display
	sdl.RenderPresent(ren)
end
