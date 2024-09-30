local color = {0x5050ff,0x00a800,0x50ffff,0xff5050,0xa800a8,0xffff00}
local Score = 0
local map = {}
local current = {x,y,type,deraction,color} --当前方块的旋转点方块坐标，类型和方向
local sq = {} --保存当前方块的每个小方块的坐标
--外层大括号为类型，内层大括号为方向，值为相对于左上角的坐标偏移量
local squres = {
  {--类型1(S)
      {-1,0,0,-1,1,-1,},--方向1
      {0,-1,1,0,1,1,},--方向2
      {1,0,0,1,-1,1,},--方向3
      {0,1,-1,0,-1,-1},--方向4
  },
  {--类型2(Z)
      {-1,-1,0,-1,1,0,},
      {1,-1,1,0,0,1},
      {1,1,0,1,-1,0},
      {-1,1,-1,0,0,-1},
  },
  {--类型3(L)
      {-1,0,1,0,1,-1},
      {0,-1,0,1,1,1},
      {1,0,-1,0,-1,1},
      {0,1,0,-1,-1,-1},
  },
  {--类型4(J)
      {-1,-1,-1,0,1,0},
      {1,-1,0,-1,0,1},
      {1,1,1,0,-1,0},
      {-1,1,0,1,0,-1},
  },
  {--类型5(I)
      {-1,0,1,0,2,0},
      {0,-1,0,1,0,2},
      {-1,0,1,0,2,0},
      {0,-1,0,1,0,2},
  },
  {--类型6(正方形)
      {1,0,0,1,1,1,},
      {1,0,0,1,1,1,},
      {1,0,0,1,1,1,},
      {1,0,0,1,1,1},
  },
  {--类型7(T)
      {-1,0,0,-1,1,0,},
      {0,-1,1,0,0,1},
      {1,0,0,1,-1,0},
      {0,1,-1,0,0,-1},
  },
}


function newsqure()--创建新的当前方块
	current.x = 5
	current.y = 2
	current.type = math.random(1,7)
	current.deraction = math.random(1,4)
	current.color = color[math.random(1,6)]

	calculate()

	if map[sq[1].x][sq[1].y] ~= 0
	or map[sq[2].x][sq[2].y] ~= 0
	or map[sq[3].x][sq[3].y] ~= 0
	or map[sq[4].x][sq[4].y] ~= 0
	then
		gameover()
	end
end

function init()--初始化
	math.randomseed(os.time())
	ege.initgraph(201,401,0)
	ege.setbkcolor(0xa0a0a0)
	ege.setcolor(0xffffff)
	ege.setfillcolor(0x0000a8)
	ege.setcaption("Tetris")
	ege.setfont(16,8,"宋体")
	
	for cloumn=1,10 do
		map[cloumn] = {}
		for raw=1,20 do
			map[cloumn][raw] = 0
		end
	end
--	map[10][1] = 1
	ege.xyprint(10,160,"Press any key to start")
	ege.getch()
	ege.cleardevice()
end

function drawmap()--画出背景(以堆叠的方块)
	for cloumn=1,10 do
		for raw=1,20 do
			if(map[cloumn][raw]~=0) then
				ege.setfillcolor(map[cloumn][raw])
				ege.bar(20*(cloumn-1),20*(raw-1),20*cloumn,20*raw)
			end
		end
	end

	ege.setcolor(0xffffff)
	for i=0,200,20 do
		ege.line(i,0,i,400)
	end
	for i=0,400,20 do
		ege.line(0,i,200,i)
	end
end

function calculate()  --计算当前方块的每个小方块的坐标
	sq[1] = {x=current.x,
					y=current.y}

	sq[2] = {x=current.x+squres[current.type][current.deraction][1],
					y=current.y+squres[current.type][current.deraction][2]}

	sq[3] = {x=current.x+squres[current.type][current.deraction][3],
					y=current.y+squres[current.type][current.deraction][4]}

	sq[4] = {x=current.x+squres[current.type][current.deraction][5],
					y=current.y+squres[current.type][current.deraction][6]}	
end

function drawcurrent()--绘制当前方块
	calculate()
	ege.setfillcolor(current.color)
	ege.bar(20*sq[1].x-20,20*sq[1].y-20,20*sq[1].x,20*sq[1].y)
	ege.bar(20*sq[2].x-20,20*sq[2].y-20,20*sq[2].x,20*sq[2].y)
	ege.bar(20*sq[3].x-20,20*sq[3].y-20,20*sq[3].x,20*sq[3].y)
	ege.bar(20*sq[4].x-20,20*sq[4].y-20,20*sq[4].x,20*sq[4].y)
end

function border()--求边界值
	local __border = {}
	__border.left = math.min(sq[1].x,sq[2].x,sq[3].x,sq[4].x)
	__border.right = math.max(sq[1].x,sq[2].x,sq[3].x,sq[4].x)
	__border.top = math.min(sq[1].y,sq[2].y,sq[3].y,sq[4].y)
	__border.bottom = math.max(sq[1].y,sq[2].y,sq[3].y,sq[4].y)

	return __border
end


function gameover()--游戏结束
	while true do
		ege.cleardevice()
		drawmap()
		ege.setfont(40,20,"宋体")
		ege.xyprint(10,200,"Game Over")
		ege.setfont(20,10,"宋体")
		ege.xyprint(40,240,"Score:"..Score)
		ege.delay_fps(60)
	end
end



function deleteline()--消行
	for raw=20,1,-1 do  --从最下面向上扫描
		local flag = true --将flag设为true，如果内层循环没遇到0，flag的值保持true不变
		for cloumn=1,10 do
			if map[cloumn][raw] == 0 then --如果这一行有0，则将flag设为false并跳出内层循环
				flag = false
				break
			end 
		end
		
		if flag then --如果flag为true,说明这一行全是1,应当消行
			Score = Score + 1
			for i=raw,2,-1 do --raw以上的行向下移动一行
				for j=1,10 do
					map[j][i] = map[j][i-1]
				end
			end
			for j=1,10 do -- 最上层清零
				map[j][1] = 0
			end
		end
	end
end

function move()
		premove = premove or os.time()
		if premove ~= os.time() then
			premove = os.time()

			if map[sq[1].x][sq[1].y+1] ~= 0 
			or map[sq[2].x][sq[2].y+1] ~= 0 
			or map[sq[3].x][sq[3].y+1] ~= 0 
			or map[sq[4].x][sq[4].y+1] ~= 0
			or border().bottom == 20 then
				map[sq[1].x][sq[1].y] = current.color
				map[sq[2].x][sq[2].y] = current.color
				map[sq[3].x][sq[3].y] = current.color
				map[sq[4].x][sq[4].y] = current.color
				newsqure()
			else
				current.y = current.y + 1
			end
		end

		if prekey == nil 
		and ege.keystate(ege.KEY_LEFT) 
		and border().left>1 
		and map[sq[1].x-1][sq[1].y] == 0
		and map[sq[2].x-1][sq[2].y] == 0
		and map[sq[3].x-1][sq[3].y] == 0
		and map[sq[4].x-1][sq[4].y] == 0
		then
			current.x = current.x - 1
			prekey = ege.KEY_LEFT
		end
		if prekey == nil 
		and ege.keystate(ege.KEY_RIGHT) 
		and border().right<10 
		and map[sq[1].x+1][sq[1].y] == 0
		and map[sq[2].x+1][sq[2].y] == 0
		and map[sq[3].x+1][sq[3].y] == 0
		and map[sq[4].x+1][sq[4].y] == 0
		then
			current.x = current.x + 1
			prekey = ege.KEY_RIGHT
		end
		if prekey ~= nil and not ege.keystate(ege.KEY_LEFT) and not ege.keystate(ege.KEY_RIGHT) then
			prekey = nil
		end

		changederaction()

end

function changederaction()
	if ege.keystate(ege.KEY_UP) and not prechange then
		prechange = true
		current.deraction = current.deraction + 1
		if current.deraction == 5 then
			current.deraction = 1
		end
		--防止旋转时超出边界
		calculate()
		if border().left<1 
		or border().right>10 
		or map[sq[1].x][sq[1].y] ~= 0
		or map[sq[2].x][sq[2].y] ~= 0
		or map[sq[3].x][sq[3].y] ~= 0
		or map[sq[4].x][sq[4].y] ~= 0
		then
			current.deraction = current.deraction - 1
			if current.deraction == 0 then
			current.deraction = 4
		end
		end
	elseif not ege.keystate(ege.KEY_UP) then
		prechange = false
	end
end
--Start at here

init()
newsqure()
while true do
	ege.cleardevice()
	move()
	deleteline()
	drawcurrent()
	drawmap()
	ege.delay_fps(60)
end
