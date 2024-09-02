-- #!/usr/local/bin/lua

function DEBUG_TRACK(msg)
	print("--------------------------")
	print("LUA Error" .. tostring(msg) .. "\n")
	print(debug.traceback())
	print("--------------------------")
end


function main()
-- 	body
	avg, sum = average(10, 20, 30, 40, 80)
	result = {avg=avg, sum=sum}
	print("The average is ", avg)
	print("The sum is ", sum)
end

local status, msg = xpcall(main, DEBUG_TRACK);
print("xpcall")
if not status then
	error(msg)
end