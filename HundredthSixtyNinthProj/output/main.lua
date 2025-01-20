function currDir()
  os.execute("cd > cd.tmp")
  local f = io.open("cd.tmp", r)
  local cwd = f:read("*a")
  f:close()
  os.remove("cd.tmp")
  return cwd
end

print('-------------------------------')
print(currDir())
print('-------------------------------')

function stringToChars(str)
    local list = {}
    local len = string.len(str)
    local i = 1 
    while i <= len do
        local c = string.byte(str, i)
        local shift = 1
        if c > 0 and c <= 127 then
            shift = 1
        elseif (c >= 192 and c <= 223) then
            shift = 2
        elseif (c >= 224 and c <= 239) then
            shift = 3
        elseif (c >= 240 and c <= 247) then
            shift = 4
        end
        local char = string.sub(str, i, i+shift-1)
        i = i + shift
        table.insert(list, char)
    end
    return list, len
end

-- function testS2C()
--   for uchar in string.gmatch(str, "[%z\1-\127\194-\244][\128-\191]*") do tab[#tab+1] = uchar end
--   local _, count = string.gsub(str, "[^\128-\193]", "")
-- end

-- print('-------------------------------')
-- testS2C()
-- print('-------------------------------')


function orderedTable(t)
  local currentIndex = 1
  local metaTable = {}
  -- 设置新值 保存键值
  function metaTable:__newindex(key,value)
    rawset(self, key, value)
    rawset(self, currentIndex, key)
    currentIndex = currentIndex + 1
  end
  return setmetatable(t or {}, metaTable)
end
function ordered(t)
  local currentIndex = 0
  -- 定义迭代器 基于索引搜索键值
  local function iter(t)
    currentIndex = currentIndex + 1
    local key = t[currentIndex]
    if key then return key, t[key] end
  end
  return iter, t
end
-- 测试数据
function show(t)
  print (t.__name .. ":")
  for k, v in ordered(t) do print(k, v) end
end

function testST()
  tab = orderedTable {__name = "table 1"}
  tab.firstName = "Rici"
  tab.lastName = "Lake"
  tab.maternalLastName = "Papert"
  show(tab)
end


print('-------------------------------')
testST()
print('-------------------------------')



function clone(object)
    -- 缓存拷贝结果
    local lookup_table = {}
    local function _copy(object)
        -- 如果不是表类型 直接返回
        if type(object) ~= "table" then
            return object
        -- 已缓存当前对象复制品 直接返回
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        -- 构建新表 用于存放复制数据
        local newObject = {}
        lookup_table[object] = newObject
        for key, value in pairs(object) do
            -- 键值一并复制
            newObject[_copy(key)] = _copy(value)
        end
        -- 拷贝对象元表
        return setmetatable(newObject, getmetatable(object))
    end
    return _copy(object)
end

-- (1) 构造一般型迭代器 自定义访问表的方式
function range(from, to, step)
  step = step or 1
  -- 迭代器返回参数：func,prevalue,lastvalue
  -- prevalue与lastvalue保存了相关状态 并作为下次迭代器调用的参数
  return function(_, lastvalue)
    local nextvalue = lastvalue + step
    -- 条件判过 迭代器继续执行 否则 停止遍历
    if step > 0 and nextvalue <= to or step < 0 and nextvalue >= to or step == 0 then
      return nextvalue
    end
  end, nil, from - step
end
function f() return 10, 0, -1 end
for i,v in range(f()) do
  print(i)
end

-- (2) 构造基于键排序的迭代器
function __genOrderedIndex( t )
    local orderedIndex = {}
    for key in pairs(t) do
        table.insert( orderedIndex, key )
    end
    table.sort( orderedIndex )
    return orderedIndex
end
-- 迭代器将上一轮的i,v作为参数赋值给state,v
function orderedNext(t, state, v)
    local key = nil
    -- 首次调用迭代器 初始化__orderedIndex
    if state == nil then
        t.__orderedIndex = __genOrderedIndex( t )
        key = t.__orderedIndex[1]
    else
        -- 返回当前state下一个值
        for i = 1,table.getn(t.__orderedIndex) do
            if t.__orderedIndex[i] == state then
                key = t.__orderedIndex[i+1]
            end
        end
    end
    if key then
        return key, t[key]
    end
    -- 遍历结束
    t.__orderedIndex = nil
    return
end
-- 首次调用 返回迭代器函数及其参数
function orderedPairs(t)
    return orderedNext, t, nil
end

-- function testIterator
--   -- 测试数据
--   tab1 = {
--       ['a'] = 'xxx',
--       ['b'] = 'xxx',
--       ['c'] = 'xxx',
--       ['d'] = 'xxx',
--       ['e'] = 'xxx',
--   }
--   print("Ordered iterating")
--   for key, val in orderedPairs(tab1) do
--       print(key.." : "..val)
--   end
-- end

-- print('-------------------------------')
-- testIterator()
-- print('-------------------------------')

-- function testRegex()
--    string.gsub("hello, up-down!", "%A", ".") -- 替换所有非字母的字符
--    select(2, string.gsub(text, "[AEIOUaeiou]", "")) -- 计算元音字母出现的次数
--    string.gsub("one, and two; and three", "%a+", "word") -- 替换所有单词
--    i,j = string.find("the number 1298 is even", "%d+") -- 计算数字的起止位置
--     test = "int x; /* x */  int y; /* y */" 
--    print(string.gsub(test, "/%*.*%*/", "newStr"))--> "int x; newStr" -- *匹配最长子串
--    print(string.gsub(test, "/%*.-%*/", "newStr"))--> "int x; newStr int y; newStr" -- -匹配最短子串
--    if string.find(s, "^%d") then end-- 字符是否以数字开头
--    if string.find(s, "^[+-]?%d+$") then end-- 字符串是否是一个整数
--    print(string.gsub("a (enclosed (in) parentheses) line", "%b()", ""))--> a  line -- 替换()内容
--    string.find("^%b<>.+%b<>$") -- 检测开头和结尾是否为<>标签
-- end

-- print('-------------------------------')
-- testRegex()
-- print('-------------------------------')


-- function testCommon()
--    for key in string.gmatch(s, "[%w%_]+%=[^%s%>]+") do -- 遍历字符串s中形如key=value的子串
--    i,j,name,value = string.find(" name = Anna ", "(%a+)%s*=%s*(%a+)") -- 括号内匹配到的字符串作为返回值，输出 2 12 name Anna
--    result = string.gsub("ahfed[9537]cd","%[[^%[%]]*%]",
--    function ( curStr )     -- 匹配包含方括号的子字符串(该字符串不允许有方括号)，并进行函数处理，将处理结果替换原子字符串
--     local i,j,value = string.find(curStr,"%[(%d+)%]")   -- 提取方括号内值
--     value=value+1
--     return value
--     end) -- result: ahfed9538cd
--    local s = "{  2d._w8(332a)|1sd3 }"
--     local _,_,v1,v2 = string.find(s,"%{%s([%a%d_%.%(%)]+)(.*)%}") -- 首先先匹配两个中括号，然后开始检索两个小括号：前一个括号使用定义的字符集(不区分顺序)检索，所有定义了的字符都将被划分至第一个匹配结果中 v1:2d._w8(332a)  v2:|1sd3
--   end
-- end

-- print('-------------------------------')
-- testCommon()
-- print('-------------------------------')


-- 生成父目录下自己的文件路径
local rootpath = ... --文件当前路径
local dotindex = string.find(rootpath, "%.%w+$") --返回路径中最后一个点号位置
local parentpath = string.sub(rootpath, 1, dotindex-1) --提取上一级文件目录
local mypath = string.format("%s.myroot.%s",parentpath,myfilename)
print('-------------------------------')
print(rootpath)
print(dotindex)
print(parentpath)
print(mypath)
print('-------------------------------')

local path = currDir()
local script = 'examples/flappybird.lua'
print('-------------------------------')
print(path)
print(script)
print('-------------------------------')
require("ejoy2d.framework").WorkDir = ''
assert(script, 'I need a script name')
path = string.match(path,[[(.*)\[^\]*$]])
package.path = path .. [[\?.lua;]] .. path .. [[\?\init.lua;.\?.lua;.\?\init.lua]]
local f = assert(loadfile(script))
f(script)