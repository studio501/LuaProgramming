
#include <stdlib.h>

#include <stdio.h>

#include <stdarg.h>

#include <string>

#include <sys/types.h>
#include <dirent.h>

#if __cplusplus
extern "C"{
#endif


#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#if __cplusplus
};
#endif


#define Max_Color 255
#define stackInLine(L) checkStackWithLine( (L),__LINE__ )

static const char *key = "koftw";
static const char *twarray = "tw.array";

struct ColorTable{
	char * name;
	unsigned char red,green,blue;
} colortable[]={
	{"White",Max_Color,Max_Color,Max_Color},
	{"Red",Max_Color,0,0},
	{"Green",0,Max_Color,0},
	{"Blue",0,0,Max_Color},
	{"Black",0,0,0},
	{NULL,0,0,0}
};


typedef struct NumArray
{
	int size;
	double values[1];
}NumArray;

void myerror(lua_State *L,const char *fmt,...)
{
	va_list argp;
	va_start(argp,fmt);
	vfprintf(stderr,fmt,argp);
	va_end(argp);

	lua_close(L);
	//exit(EXIT_FAILURE);
	system("pause");
}


void cleanStack(lua_State *L)
{
	lua_pop(L,lua_gettop(L));
}

void checkStack(lua_State *L)
{
	for(int i=1;i<=lua_gettop(L);++i)
	{
		int t = lua_type(L,i);
		switch (t)
		{
		case LUA_TSTRING:
			printf("'%s '",lua_tostring(L,i));
			break;
		case LUA_TNUMBER:
			printf("%g ",lua_tonumber(L,i));
			break;
		case LUA_TBOOLEAN:
			printf(lua_toboolean(L,i)?"true ":"false ");
			break;
		default:
			{
				if(lua_isnil(L,i)){
					printf("nil ");
				}else if(lua_istable(L,i)){
					printf("table ");
				}
				else if(lua_isuserdata(L,i)){
					printf("user_data ");
				}
				else{
					printf("unknow ");
				}
			}
			break;
		}
	}
		
	printf("\n");
}

void checkStackWithLine(lua_State *L,int line)
{
	printf("Line :%d ",line);
	checkStack(L);
}

//假定表已经被push到栈上了
int getfield(lua_State *L,const char *key)
{
	int result;
	lua_pushstring(L,key);
	lua_gettable(L,-2);
	result = lua_tonumber(L,-1);
	lua_pop(L,1);
	return result;
}

//假定表已经被push到栈上
void setfield(lua_State *L,const char *key,int value)
{
	lua_pushstring(L,key);
	lua_pushnumber(L,value);
	lua_settable(L,-3);
}

void setcolor(lua_State *L,struct ColorTable *ct)
{
	lua_pop(L,lua_gettop(L));
	
	lua_newtable(L);
	setfield(L,"r",ct->red);
	setfield(L,"g",ct->green);
	setfield(L,"b",ct->blue);
	lua_setglobal(L,ct->name);

}

double mathFunc(lua_State *L,double x,double y)
{
	double z;

	cleanStack(L);
	
	lua_getglobal(L,"f");
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);

	if(lua_pcall(L,2,2,0)!=0)
		myerror(L,"call func failed");
	checkStack(L);
	if(!lua_isnumber(L,-1))
		myerror(L,"result error");
	z = lua_tonumber(L,-1);
	lua_pop(L,2);
	checkStack(L);
	return z;
}

//通用函数调用
void call_va(lua_State *L,const char *func,const char *sig,...)
{
	cleanStack(L);
	va_list vl;
	int nargs,nres;//number of args and results
	va_start(vl,sig);
	lua_getglobal(L,func);

	//push arguments
	nargs = 0;
	while(*sig){
		switch (*sig++)
		{
		case 'd':
			lua_pushnumber(L,va_arg(vl,double));
			break;
		case 'i':
			lua_pushnumber(L,va_arg(vl,int));
			break;
		case 's':
			lua_pushstring(L,va_arg(vl,char*));
			break;
		case '>':
			goto endwhile;
		default:
			myerror(L,"invalid arguments");
			break;
		}
		++nargs;
	}endwhile:

	//do the call
	nres = strlen(sig);
	if(lua_pcall(L,nargs,nres,0)!=0)
		myerror(L,"call failed");
	//retrieve result

	nres = -nres;
	
	while(*sig)
	{
		switch (*sig++)
		{
		case 'd':
			if(!lua_isnumber(L,nres))
				myerror(L,"wrong result type");
			*va_arg(vl,double*) = lua_tonumber(L,nres);
			break;
		case 'i':
			if(!lua_isnumber(L,nres))
				myerror(L,"wrong result type");
			*va_arg(vl,int *) = lua_tonumber(L,nres);
			break;
		case 's':
			if(!lua_isstring(L,nres))
				myerror(L,"wrong result type");
			*va_arg(vl,const char **) = lua_tostring(L,nres);
			break;
		default:
			myerror(L,"unknow return type");
			break;
		}
		++nres;
	}
	va_end(vl);
}

void loadScript(lua_State *L,char *filename)
{
	cleanStack(L);
	if(luaL_loadfile(L,filename) || lua_pcall(L,0,0,0)){
		myerror(L,"can't run file %s",lua_tostring(L,-1));
	}
}

//读取lua配置
void load(lua_State *L,char *filename,int *width,int *height)
{
	loadScript(L,filename);
	/*if(luaL_loadfile(L,filename) || lua_pcall(L,0,0,0)){
		myerror(L,"can't run file %s",lua_tostring(L,-1));
	}*/
	lua_getglobal(L,"width");
	lua_getglobal(L,"height");
	if(!lua_isnumber(L,-2) || !lua_isnumber(L,-1))
		myerror(L,"number is invalid");
	*width = lua_tonumber(L,-2);
	*height =lua_tonumber(L,-1);
	lua_pop(L,2);

	lua_getglobal(L,"background");
	if(!lua_istable(L,-1)){
		myerror(L,"invalid table");
	}
	printf("(%d,%d,%d)\n",getfield(L,"r"),getfield(L,"g"),getfield(L,"b"));
	//checkStack(L);
	int i=0;
	while(colortable[i].name!=NULL)
	{
		setcolor(L,&colortable[i++]);
	}
	
	
	lua_getglobal(L,"White");
	printf("(%d,%d,%d)\n",getfield(L,"r"),getfield(L,"g"),getfield(L,"b"));
	cleanStack(L);
	
	printf("math return is %f\n",mathFunc(L,0.1,0.2));

	double x,y,x1=0.0,y1=0.0;
	x=0.1;
	y=0.6;
	call_va(L,"f","dd>dd",x,y,&x1,&y1);
	printf("x1 is %g y1 is %g\n",x1,y1);

	char *str;
	call_va(L,"numberToStrint","i>s",1000,&str);
	
	//call_va(L,"nothing",">");
}

//sin 函数 for lua
static int l_sin(lua_State *L)
{
	double d = lua_tonumber(L,1);
	luaL_checknumber(L,1);
	lua_pushnumber(L,sin(d));
	return 1;
}


//check dir for lua
static int l_dir(lua_State *L)
{
	DIR * dir;
	struct dirent * entry;
	int i;
	//checkStackWithLine(L,__LINE__);
	const char *path = luaL_checkstring(L,1);

	//open directory
	dir = opendir(path);
	if(dir==NULL)
	{
		lua_pushnil(L);
		lua_pushstring(L,strerror(errno));
		return 2;
	}
	//checkStackWithLine(L,__LINE__);
	//create result table
	lua_newtable(L);
	//checkStackWithLine(L,__LINE__);
	i=1;
	while ((entry = readdir(dir))!=NULL)
	{
		/*lua_pushnumber(L,i++);
		lua_pushstring(L,entry->d_name);
		lua_settable(L,-3);*/
		lua_pushstring(L,entry->d_name);
		lua_rawseti(L,-2,i++);
	}
	closedir(dir);
	return 1;
}


static int l_getPerson(lua_State *L)
{
	luaL_checknumber(L,-2);
	luaL_checkstring(L,-1);
	
	int age = (int)lua_tonumber(L,-2);
	const char * name = lua_tostring(L,-1);

	lua_newtable(L);
	lua_pushstring(L,"age");
	lua_pushnumber(L,age);
	lua_settable(L,-3);

	lua_pushstring(L,"name");
	lua_pushstring(L,name);
	lua_settable(L,-3);

	return 1;
}

//check arr
static int l_checkarr(lua_State *L)
{

	lua_newtable(L);

	lua_pushnumber(L,1);
	lua_rawseti(L,-2,1);
	lua_pushnumber(L,100);
	lua_rawseti(L,-2,2);

	lua_pushnumber(L,1000);
	lua_pushnumber(L,3);
	lua_insert(L,-2);

	lua_rawset(L,-3);

	lua_pushnumber(L,99);
	lua_rawseti(L,-2,4);

	lua_pushstring(L,"this is a string");
	lua_rawseti(L,-2,5);

	lua_newtable(L);
	lua_pushstring(L,"a");
	lua_rawseti(L,-2,1);
	lua_pushnumber(L,78);
	lua_rawseti(L,-2,2);
	lua_rawseti(L,-2,6);
	stackInLine(L);
	return 1;
}

//
static int l_map(lua_State *L)
{
	int i,n;
	luaL_checktype(L,1,LUA_TTABLE);
	luaL_checktype(L,2,LUA_TFUNCTION);

	n = luaL_getn(L,1);

	for(i=1;i<=n;++i)
	{
		lua_pushvalue(L,2);
		lua_rawgeti(L,1,i);
		lua_call(L,1,1);
		lua_rawseti(L,1,i);
	}
	return 0;
}

//
static int l_split(lua_State *L)
{
	const char *s = luaL_checkstring(L,1);
	const char *sep = luaL_checkstring(L,2);
	const char *e;
	int i=1;
	lua_newtable(L);

	while((e=strchr(s,*sep))!=NULL)
	{
		lua_pushlstring(L,s,e-s);
		lua_rawseti(L,-2,i++);
		s = e+1;
	}
	lua_pushstring(L,s);
	lua_rawseti(L,-2,i);
	return 1;
}

//
static int ctostring(lua_State *L)
{
	lua_concat(L,lua_gettop(L));
	return 1;
}

//registry
static int registryTest(lua_State *L)
{
	lua_pushlightuserdata(L,(void *)&key);
	lua_pushnumber(L,10077);
	lua_settable(L,LUA_REGISTRYINDEX);
	
	lua_pushlightuserdata(L,(void*)&key);
	lua_gettable(L,LUA_REGISTRYINDEX);
	return 1;
}

//
static int counter(lua_State *L)
{
	double val = lua_tonumber(L,lua_upvalueindex(1));
	lua_pushnumber(L,++val);
	lua_pushvalue(L,-1);
	lua_replace(L,lua_upvalueindex(1));
	return 1;
}

static int newCountter(lua_State *L)
{
	lua_pushnumber(L,0);
	lua_pushcclosure(L,counter,1);
	return 1;
}

//
static NumArray * checkArray(lua_State *L)
{
	void *ud = luaL_checkudata(L,1,twarray);
	luaL_argcheck(L,ud!=NULL,1,"array expected");
	return (NumArray *)ud;
}

//
static double * getelem(lua_State *L)
{
	NumArray *a = checkArray(L);
	int index = luaL_checkint(L,2);

	luaL_argcheck(L,1<=index && index<=a->size,2,"wrong range");

	return &a->values[index-1];
}

//
static int newArray(lua_State *L)
{
	int n = luaL_checkint(L,1);
	size_t nbytes = sizeof(NumArray) + (n-1)*sizeof(double);
	NumArray *a = (NumArray *)lua_newuserdata(L,nbytes);

	luaL_getmetatable(L,twarray);
	lua_setmetatable(L,-2);


	a->size = n;
	return 1;
}

static int setArray(lua_State *L)
{
	/*NumArray *a = (NumArray *)lua_touserdata(L,1);
	int index = luaL_checkint(L,2);
	double value = luaL_checknumber(L,3);

	luaL_argcheck(L,a!=NULL,1,"array expected");
	luaL_argcheck(L,1<=index && index<=a->size,2,"wrong range");

	a->values[index-1] = value;*/
	double value = luaL_checknumber(L,3);
	*getelem(L) = value;
	return 0;
}

static int getArray(lua_State *L)
{
	/*NumArray * a = (NumArray *)lua_touserdata(L,1);
	int index = luaL_checkint(L,2);

	luaL_argcheck(L,a!=NULL,1,"array expected");
	luaL_argcheck(L,1<=index && index<=a->size,2,"wrong arange");
	lua_pushnumber(L,a->values[index-1]);*/
	lua_pushnumber(L,*getelem(L));
	return 1;
}

static int getArrayLen(lua_State *L)
{
	NumArray *a = checkArray(L);
		//(NumArray *)lua_touserdata(L,1);

	luaL_argcheck(L,a!=NULL,1,"array expected");

	lua_pushnumber(L,a->size);
	return 1;
}

//
static int array2string(lua_State *L)
{
	NumArray *a = checkArray(L);
	lua_pushfstring(L, "array(%d)", a->size);
	return 1;
}


int luaopen_array(lua_State *L)
{
	cleanStack(L);
	static const struct luaL_reg mylib_f[] = {
		{"new",newArray},
		{NULL,NULL},
	};

	static const struct luaL_reg mylib_m[] = {
		{"set",setArray},
		{"get",getArray},
		{"size",getArrayLen},
		{ "__tostring",array2string },
		{NULL,NULL},
	};
	
	luaL_newmetatable(L,twarray);
	lua_pushstring(L,"__index");
	lua_pushvalue(L,-2);
	lua_settable(L,-3); //metatable.__index = metatable

	//stackInLine(L);
	luaL_openlib(L,NULL,mylib_m,0);
	
	luaL_openlib(L,"array",mylib_f,0);

	//stackInLine(L);
	
	/*lua_pushstring(L, "__index");
	lua_pushstring(L, "get");
	lua_gettable(L, 2);
	lua_settable(L, 2);*/
	return 1;
}

int luaopen_oriarray(lua_State *L)
{
	cleanStack(L);
	static const struct luaL_reg mylib_m1[] = {
		{ "new",newArray },
		{ "set",setArray },
		{ "get",getArray },
		{ "size",getArrayLen },
		{ "__tostring",array2string },
		{ NULL,NULL },
	};

	luaL_newmetatable(L, "tworiarray");
	luaL_openlib(L, "oriarray", mylib_m1,0);

	lua_pushstring(L, "__index");
	lua_pushstring(L, "get");
	lua_gettable(L, 2);
	lua_settable(L, 1);

	lua_pushstring(L, "__newindex");
	lua_pushstring(L, "set");
	lua_gettable(L, 2);
	lua_settable(L, 1);
	return 0;
}

//open my lib
int luaopen_mylib(lua_State *L)
{
	
	static const struct luaL_reg mylib[] = {
		{"mydir",l_dir},
		{"mysin",l_sin},
		{"myperson",l_getPerson},
		{"checkarr",l_checkarr},
		{"initmap",l_map},
		{"splitstring",l_split},
		{"ctostring",ctostring},
		{"registryTest",registryTest},
		{"newCountter",newCountter},
		{NULL,NULL},
	};

	luaL_openlib(L,"mylib",mylib,0);
	return 1;
}

void main()
{
	char buf[256];
	int error;
	lua_State *L = lua_open();//open lua
	luaopen_base(L);//open the basic library
	luaopen_table(L);//open the table library
	
	lua_pushcfunction(L,luaopen_io);
	lua_pushstring(L,LUA_IOLIBNAME);
	lua_call(L,1,0);

	//luaopen_io(L);//open the io library
	luaopen_string(L);//open the string libray
	luaopen_math(L);//open the math libray
	
	lua_pop(L,lua_gettop(L));


	lua_pushstring(L,"good");
	printf("str is %s\n",lua_tostring(L,lua_gettop(L)));
	lua_pushnumber(L,100);
	lua_pushboolean(L,false);


	lua_pushvalue(L,-3);
	lua_replace(L,2);
	lua_settop(L,6);
	lua_remove(L,-2);
	lua_settop(L,-5);
	checkStack(L);
	cleanStack(L);

	//注册函数
	printf("begin to regester funciton\n");
	/*lua_pushcfunction(L,l_sin);
	lua_setglobal(L,"mysin");

	lua_pushcfunction(L,l_dir);
	lua_setglobal(L,"mydir");*/
	luaopen_mylib(L);
	luaopen_array(L);
	luaopen_oriarray(L);

	printf("end to regester funciton\n");
	
	int w=0,h=0;
	//load(L,"window.lua",&w,&h);
	//printf("w is %d,h is %d",w,h);

	loadScript(L,"window.lua");


	

	lua_close(L);

	//system("pause");
	getchar();
}