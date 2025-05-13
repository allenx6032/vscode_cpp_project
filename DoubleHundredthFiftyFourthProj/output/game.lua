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
	local time = pixl.time() -- return the passed time sice start in seconds
	print(time)
	Sleep2(2)

	pixl.resolution(900, 900) -- set resolution to 256x240 (NES screen size)
	pixl.resolution(900, 600, 3.0 / 2.0) -- set the resolution to 320x200 (famous DOS VGA) and also set the aspect ratio to 4:3
	Sleep2(2)

	pixl.translate(-10, -10) -- set translation to -10,-10
	pixl.point(1, 10, 10) -- this pixel will now appear on 0,0
	local x, y = pixl.translate() -- get current translation
	
	print(x .. "  " .. y)
	Sleep2(2)

	x, y = pixl.mouse()
	print(x .. "  " .. y)
	Sleep2(2)
	
	local down_A = pixl.btn('A') -- returns true if button 'A' is currently down
	local pressed_A = pixl.btnp('A') -- returns true if button 'A' was pressed once since last frame
	
	--print(down_A .. "  " .. pressed_A)
	Sleep2(2)
	
	pixl.randomseed(47) -- set a new random seed
	pixl.randomseed(os.time()) -- set a more "random" seed :)
	local seed = pixl.randomseed() -- get current seed
	
	print(seed)
	Sleep2(2)
	pixl.quit() -- end the event-loop
end

main()
