local turtle = require("turtle")

function polygon(side)
	if side < 3 then
		error("side must bigger than 3" ,2)
	end
	for i = 1,side do
		turtle.go(50)
		turtle.left(360/side)
	end
end

turtle.start()
turtle.teleport(275,580)
for i = 3,24 do
	polygon(i)
end
turtle.wait()