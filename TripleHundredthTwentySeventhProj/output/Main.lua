print "Carga de main"
dofile( "./ExampleEntity.lua" )
dofile( "./ReBounder.lua" )

function Start()
	
	for i=0, 10 do
	ExampleEntity:new()
	end
	
	arg0 = 10
	arg1 = 20
	ReBounder:new()
	arg0 = 100
	arg1 = 200
	ReBounder:new()
	arg0 = 200
	arg1 = 200
	ReBounder:new()
	arg0 = 500
	arg1 = 100
	ReBounder:new()
	arg0 = 177
	arg1 = 77
	ReBounder:new()
	
end

