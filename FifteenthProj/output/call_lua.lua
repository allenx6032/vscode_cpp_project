#!/usr/local/bin/lua

--[[
	通过require引入的文件，会自动把这个文件load进来，通过当前文件的路径注意
	注意，这里不能用require("call_lua_third.lua")来require文件
--]]
require("call_lua_third")

print_hello()

background = {r = 0.3, g = 0.4 , b = 0.5}