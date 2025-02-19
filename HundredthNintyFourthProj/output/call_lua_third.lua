#!/usr/local_bin/lua

function print_hello()
	-- body
	print("hello, call_lua_third")
end

function addInt(x, y)
	-- body
	sum = x + y
	print("lua addInt function : " .. sum )
	return x + y
end