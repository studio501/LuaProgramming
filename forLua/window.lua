width = 100
height = 200
background = {r=255,g=0,b=9}

--local mylib = require("mylib")

function cclog(...)
	print(string.format(...))
end

function tab_n(tabcount,str)
	return string.format("%s%s",string.rep(" ",tabcount),str)
end

function printTable(t,fromInner,tab)
	if not (type(t) == "table") then
		-- print(t)
		cclog(tostring(t))
	else
		tab = tab or 0
		cclog(tab_n( tab,string.format("%s=>{",fromInner or "table")))
		
		for key,var in pairs(t) do
			if not (type(var) == "table") then
				cclog(tab_n( tab+1*4,string.format("%s=>%s",key,tostring( var) )))
			else
				--cclog("%s:{",key)
				printTable(var,key,tab+1*4)
				--cclog("}")
			end
		end	
		
		cclog(tab_n(tab,"}"))
	end
	
end

function f(x,y)
	return x + y,x-y
end

function numberToStrint(s)
	return tostring(s)
end

function tmap(var)
	return var*2
end

function mathmeticAndRelationMetatable(  )
	local t1 = {}
	local t = {}

	setmetatable(t,t1)

	local Set = {}

	Set.mt = {}

	


	function Set.new( t )
		local set = {}
		setmetatable(set,Set.mt)
		for _,l in ipairs(t) do
			set[l] = true
		end
		return set
	end

	function Set.union( a,b )
		local res = Set.new({})
		for k in pairs(a) do
			res[k] = true
		end

		for k in pairs(b) do
			res[k] = true
		end
		return res
	end

	function Set.intersection( a,b )
		local res = Set.new{}
		for k in pairs(a) do
			res[k] = b[k]
		end
		return res
	end

	function Set.sub( a,b )
		local res = Set.new{}
		for k in pairs(a) do
			if not b[k] then
				res[k] = a[k]
			end
		end
		return res
	end

	function Set.div( a,b )
		return Set.sub(a,b)
	end


	function Set.mt.__le( a,b )
		for k in pairs(a) do
			if not b[k] then
				return false
			end
		end
		return true
	end

	function Set.mt.__lt( a,b )
		return a<=b and not(b<=a)
	end

	function Set.mt.__eq( a,b )
		return a<=b and b<=a
	end


	Set.mt.__add = Set.union
	Set.mt.__mul = Set.intersection
	Set.mt.__sub = Set.sub
	Set.mt.__div = Set.div
	Set.mt.__tostring = function ( set )
		local s = "{"
		local sep = ""
		for e in pairs(set) do
			s = s..sep..e
			sep = ", "
		end
		return s.."}"
	end
	Set.mt.__metatable = "not your business"



	local s1 = Set.new{10,20,30,50}
	local s2 = Set.new{30,20}

	-- print(getmetatable(s1))
	-- print(getmetatable(s2))
	local s3 = s1 + s2
	-- printTable(s3)
	local s4 = s1*s2
	-- printTable( s3*s1 )
	print(s1-s2)
	cclog(getmetatable(s1))
end

function capi(  )
	print( mylib.mysin(math.pi/2))
	printTable( mylib.mydir("."))
	printTable(mylib.myperson(24,"tangwen"));
	printTable(mylib.checkarr());
	local t = {1,2,3,4,100}
	mylib.initmap(t,tmap)
	printTable(t)
	
	printTable(mylib.splitstring("asf,dfe,er,,fdad,ggg",","))
	
	print(mylib.ctostring(1,2,3,"abc"))
	
	print(mylib.registryTest())
	
	local ct1 = mylib.newCountter()
	local ct2 = mylib.newCountter()
	
	for i=1,3 do
		print(ct1())
		print(ct2())
	end

	


	local a = array.new(100)
	a:set(1,100)
	cclog("%s .. %s",a:get(1),a:size())
	print(a)

	local b = oriarray.new(100)
	b:set(1,100)
	cclog(b:get(1))
end

function indexMetatable(  )
	local Window = {}
	Window.prototype = {x=0,y=0,width=100,height=100}

	Window.mt = {}

	function Window.new( t )
		setmetatable(t,Window.mt)
		return t
	end

	-- function Window.mt.__index( t,key )
	-- 	return Window.prototype[key]
	-- end

	Window.mt.__index = Window.prototype
	Window.mt.__newindex = Window.prototype

	local w = Window.new{}
	print(w.width)

	local x = Window.new{}
	x.height = 200
	print(x.height)

	local z = Window.new{}
	print(z.height)
end

function weaktb(  )
	local a,b = {},{}
	setmetatable(a,b)
	b.__mode = "k"

	local key = {}
	a[key] = 1

	key = {}
	a[key] = 2

	local key3 = {}
	a[key3] = 3

	for k,v in pairs(a) do
		print(v)
	end

	collectgarbage()

	cclog("after collectgarbage")
	for k,v in pairs(a) do
		print(v)
	end
end

function binaryFile(  )
	-- local out = assert(io.open("food","wb"))
	-- local str = "food is good"
	
	-- out:write(string.byte(str,1,#str))	
	
	local inf = assert(io.open("food","rb"))
	local data = inf:read("*all")

	-- print(string.char())
	-- assert(out:close())
	assert(inf:close())
end

function osAndEnv(  )
	print(os.getenv("path"))
	os.execute("mkdir ./fooddir")
end

function main()
	print("main")
	-- capi()
	-- mathmeticAndRelationMetatable()
	-- indexMetatable()
	-- weaktb()
	-- binaryFile()
	osAndEnv()
end

main()