--[[
	通过require引入的文件，会自动把这个文件load进来，通过当前文件的路径注意
	注意，这里不能用require("call_lua_third.lua")来require文件
--]]

--print(my_functions_registry.mul(7, 8))

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

print("hello from lua")

print_hello()

addInt(11,22)