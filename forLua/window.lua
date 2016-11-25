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
		print(t)
	else
		tab = tab or 0
		cclog(tab_n( tab,string.format("%s=>{",fromInner or "table")))
		
		for key,var in pairs(t) do
			if not (type(var) == "table") then
				cclog(tab_n( tab+1*4,string.format("%s=>%s",key,var)))
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

function main()
	print("main")
	
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

	
	
	
	-- local newarr = mylib.newArr(100)
	
	
	
	
	-- mylib.setArr(newarr,1,1)
	-- mylib.setArr(newarr,2,2)
	-- mylib.setArr(newarr,3,3)
	-- mylib.setArr(newarr,4,4)
	
	-- cclog("%s  len is %s",mylib.getArr(newarr,2),mylib.sizeArr(newarr))
	
	-- newarr:set(5,5)
	-- cclog("%s ",newarr:size())

	local a = array.new(100)
	a:set(1,100)
	cclog("%s .. %s",a:get(1),a:size())
end

main()