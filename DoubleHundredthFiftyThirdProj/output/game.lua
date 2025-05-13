local pixl = require('pixl')

function init()
  -- load all needed assets and create game data etc.
end

function update(dt)
  pixl.clear()
  pixl.print(15, 0, 0, string.format('dt: %f - FPS: %f', dt, 1.0 / fps))
end

function Sleep1(n)
	print("Sleep1")
    local t0 = os.clock()
    while os.clock() - t0 <= n do end
end

function Sleep2(n)
	print("Sleep2")
    --os.execute("sleep " .. n)
    os.execute("timeout /t " .. n)
end

-- function Sleep3(n)
-- 	print("Sleep3")
--     if n > 0 then os.execute("ping -n " .. tonumber(n + 1) .. " localhost > NUL") end
-- end


function main()
	pixl.title('My awesome game!') -- set the window title
	local title = pixl.title() -- get current window title
	local time = pixl.time() -- return the passed time sice start in seconds

	print(time)
	Sleep2(3)

	pixl.resolution(900, 900) -- set resolution to 256x240 (NES screen size)
	pixl.resolution(900, 600, 3.0 / 2.0) -- set the resolution to 320x200 (famous DOS VGA) and also set the aspect ratio to 4:3
	local width, height = pixl.resolution() -- get current resolution (this is a quick call)

	print(width .. "  " .. height)
	Sleep2(3)

	pixl.translate(-10, -10) -- set translation to -10,-10
	pixl.point(1, 10, 10) -- this pixel will now appear on 0,0
	local x, y = pixl.translate() -- get current translation
	
	print(x .. "  " .. y)
	Sleep2(3)

	pixl.clip(10, 10, 50, 50) -- restrict drawing to the area of 10,10-50,50
	local w, h = pixl.resolution()
	
	print(w .. "  " .. h)
	Sleep2(3)
	
	pixl.clip(0, 0, w - 1, h - 1) -- reset clipping to the whole screen
	local x1, y1, x2, y2 = pixl.clip() -- get current clipping area

	print(x1 .. "  " .. y1 .. "  " .. x2 .. "  " .. y2)
	Sleep2(3)

	x, y = pixl.mouse()
	pixl.sprite(x, y, 4, 4, '\0\1\2\3\4\5\6\7\8\9\10\11\12\13\14\15') -- draw a 4x4 image containing the first 16 colors
	
	print(x .. "  " .. y)
	Sleep2(3)
	
	local down_A = pixl.btn('A') -- returns true if button 'A' is currently down
	local pressed_A = pixl.btnp('A') -- returns true if button 'A' was pressed once since last frame
	
	--print(down_A .. "  " .. pressed_A)
	Sleep2(3)
	
	pixl.randomseed(47) -- set a new random seed
	pixl.randomseed(os.time()) -- set a more "random" seed :)
	local seed = pixl.randomseed() -- get current seed
	
	print(seed)
	Sleep2(3)
	pixl.quit() -- end the event-loop
end

main()
