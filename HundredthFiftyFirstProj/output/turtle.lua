local sdl = require("libsdluna")
local turtle = {}


function turtle.toint(n)
	if n > 0 then
		return math.modf(n + 0.5)
	elseif n < 0 then
		return math.modf(n - 0.5)
	else
		return 0
	end
end

function turtle.start(width,height)
	if turtle.__inited then
		return false
	end
	turtle.__inited = true
	turtle.__w = width or 600
	turtle.__h = height or 600
	turtle.__x = turtle.__w / 2
	turtle.__y = turtle.__h / 2
	turtle.__angle = 0
	turtle.__win = sdl.CreateWindow("SDLuna Turtle Graphics", 
										sdl.WINDOWPOS_UNDEFINED, 
										sdl.WINDOWPOS_UNDEFINED,
										turtle.__w, turtle.__h, 
										sdl.WINDOW_SHOWN)
										
	turtle.__ren = sdl.CreateRenderer(turtle.__win, -1, sdl.RENDERER_SOFTWARE)
	sdl.SetRenderDrawColor(turtle.__ren, 0,0,0,0)
	sdl.RenderClear(turtle.__ren)
	sdl.SetRenderDrawColor(turtle.__ren, 0,0xff,0,0)
	sdl.Point(turtle.__ren, turtle.__x, turtle.__y)
	sdl.RenderPresent(turtle.__ren)
	
	return true
end

function turtle.wait()
	local event = nil
	while event == nil or event.type ~= sdl.EVENT_QUIT do
		event = sdl.PollEvent()
		sdl.RenderPresent(turtle.__ren)
	end
end

function turtle.go(pix)
	if pix < 0 then
		error("bad argument #1 (Turtle cannot walk backwards)", 2)
	end
	local counter = sdl.CreateCounter()
	local dstX
	local dstY
	for n=1,pix,1 do
		dstX = turtle.toint(turtle.__x + n * math.cos(math.rad(turtle.__angle)))
		dstY = turtle.toint(turtle.__y + n * math.sin(math.rad(turtle.__angle)))
		sdl.SetRenderDrawColor(turtle.__ren, 0xe9,0x1e,0x63,0)
		sdl.Line(turtle.__ren, turtle.__x, turtle.__y, dstX, dstY)
		sdl.SetRenderDrawColor(turtle.__ren, 0,0xff,0,0)
		sdl.Point(turtle.__ren, dstX, dstY)
		sdl.RenderPresent(turtle.__ren)
		
		while not counter:time(10) do
			local event = sdl.PollEvent()
			if event ~= nil and event.type == sdl.EVENT_QUIT then
				os.exit()
			end
		end
		
		sdl.SetRenderDrawColor(turtle.__ren, 0xe9,0x1e,0x63,0)
		sdl.Point(turtle.__ren, dstX, dstY)
	end
	
	turtle.__x = dstX
	turtle.__y = dstY
end

function turtle.right(angle)
	turtle.__angle = turtle.__angle + angle
	turtle.__angle = turtle.__angle % 360
end

function turtle.left(angle)
	turtle.__angle = turtle.__angle - angle
	turtle.__angle = turtle.__angle % 360
end

function turtle.teleport(x, y)
	sdl.SetRenderDrawColor(turtle.__ren, 0,0,0,0)
	sdl.Point(turtle.__ren, turtle.__x, turtle.__y)
	turtle.__x = x
	turtle.__y = y
end

return turtle