--[[
	通过require引入的文件，会自动把这个文件load进来，通过当前文件的路径注意
	注意，这里不能用require("call_lua_third.lua")来require文件
--]]
require("call_lua_third")

print_hello()

print("hello from lua")

local a,b = 2,3
print(a*b)

function DEBUG_TRACK(msg)
	print("--------------------------")
	print("LUA Error" .. tostring(msg) .. "\n")
	print(debug.traceback())
	print("--------------------------")
end

background = {r = 0.3, g = 0.4 , b = 0.5}

print(1)

print(2)