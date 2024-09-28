/* Headfile of Lua */
#include "lua_includes.h"
/* Headfile of Ege */
#include "ege.h"
#include "ege_graphics.h"
/*Simple headfile*/
#include <stdint.h>

#include "ege_lua.h"

/* 辅助函数 */
static inline void ege4lua_error(const char* fname,lua_State* L,const char* str = "")//报错并退出
{
	luaL_error(L,"\nError : function [%s] meets a fatal error.\n        %s",fname,str);
}

static inline void ege4lua_warning(const char* fname,const char* str = "")//产生警告
{
	printf("Warning : function [%s] meets a bug.\n          %s",fname,str);
}

static inline int ege4lua_tointeger(const char* fname,lua_State* L,int index)//读取一个数值转换成int返回
{
	double n = luaL_checknumber(L,index);
	if(n != (static_cast<int>(n)))
	{
		ege4lua_warning(fname,"parameters of pixel or color is not integer.\n");
	}
	return static_cast<int>(n);
}

static int ege4lua_getintarray(const char* fname,lua_State* L,int* array = NULL)//读一个数组参数
{
	int length = luaL_len(L,1);
	int index = 1;
	for(index = 1 ; index <= length ; index ++)
	{
		lua_pushinteger(L,index);
		lua_gettable(L,1);
		if(array != NULL)
		{
			array[index-1] = ege4lua_tointeger(fname,L,-1);
			lua_pop(L,1);//将前面gettable放在栈顶的元素弹出
		}
	}

	return length;
}



/* 绘图环境相关函数 */
int ege4lua_cleardevice(lua_State* L)
{
	if(lua_type(L,1)==LUA_TNONE)
		cleardevice();
	else if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
		cleardevice(static_cast<ege::IMAGE*>(lua_touserdata(L,1)));
	else
		ege4lua_error(__FUNCTION__,L,"parameter is incorrect.\n");

	return 0;
}

int ege4lua_clearviewport(lua_State* L)
{
	if(lua_type(L,1)==LUA_TNONE)
		clearviewport();
	else if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
		clearviewport(static_cast<ege::IMAGE*>(lua_touserdata(L,1)));
	else
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");

	return 0;
}

int ege4lua_closegraph(lua_State* L)
{
	if(lua_type(L,1) != LUA_TNONE)
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	else
		closegraph();

	return 0;
}

int ege4lua_gettarget(lua_State* L)
{
	if(lua_type(L,1) != LUA_TNONE)
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	else
	{
		lua_pushlightuserdata(L,static_cast<void*>(gettarget()));
	}

	return 1;
}

int ege4lua_getviewport(lua_State* L)
{
	if(lua_type(L,1)!=LUA_TNONE)
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
		return 0;
	}
	else
	{
		int pleft;
		int ptop;
		int pright;
		int pbottom;
		int pclip;
		PIMAGE pimg = newimage(pright-pleft,pbottom-ptop);

		getviewport(&pleft,&ptop,&pright,&pbottom,&pclip,pimg);
		lua_newtable(L);//创建一张表压栈

		lua_pushstring(L,"img");
		lua_pushlightuserdata(L,static_cast<void*>(pimg));
		lua_settable(L,-3);

		lua_pushstring(L,"left");
		lua_pushinteger(L,pleft);
		lua_settable(L,-3);

		lua_pushstring(L,"top");
		lua_pushinteger(L,ptop);
		lua_settable(L,-3);

		lua_pushstring(L,"right");
		lua_pushinteger(L,pright);
		lua_settable(L,-3);

		lua_pushstring(L,"bottom");
		lua_pushinteger(L,pbottom);
		lua_settable(L,-3);

		lua_pushstring(L,"clip");
		lua_pushinteger(L,pclip);
		lua_settable(L,-3);

		return 1;
	}
}


int ege4lua_initgraph(lua_State* L)
{
	if(lua_type(L,3) == LUA_TNONE)
	{
		initgraph(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2));
	}
	else
	{
		initgraph(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2),
					ege4lua_tointeger(__FUNCTION__,L,3));
	}

	setcaption("EGE for Lua");

	return 0;
}


int ege4lua_isrun(lua_State* L)
{
	if(lua_type(L,1)!=LUA_TNONE)
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	lua_pushboolean(L,is_run());
	return 1;
}


int ege4lua_setactivepage(lua_State* L)
{
	int page = ege4lua_tointeger(__FUNCTION__,L,1);
	if(page<0 || page>3)
		lua_pushboolean(L,false);
	else
	{
		setactivepage(page);
		lua_pushboolean(L,true);
	}
	return 1;
}


int ege4lua_setcaption(lua_State* L)
{
	setcaption(luaL_checkstring(L,1));
	return 0;
}


int ege4lua_setinitmode(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setinitmode(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,3) == LUA_TNONE)
	{
		setinitmode(ege4lua_tointeger(__FUNCTION__,L,1),ege4lua_tointeger(__FUNCTION__,L,2));
	}
	else
	{
		setinitmode(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2),
					ege4lua_tointeger(__FUNCTION__,L,3));
	}

	return 0;
}


int ege4lua_setrendermode(lua_State* L)
{
	setrendermode(static_cast<rendermode_e>(ege4lua_tointeger(__FUNCTION__,L,1)));
	return 0;
}


int ege4lua_settarget(lua_State* L)
{
	if(lua_type(L,1) == LUA_TNONE)
	{
		settarget(NULL);
	}
	else if(lua_type(L,1) == LUA_TLIGHTUSERDATA)
	{
		settarget(static_cast<PIMAGE>(lua_touserdata(L,1)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_setviewport(lua_State* L)
{
	if(lua_type(L,5) == LUA_TNONE)
	{
		setviewport(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2),
					ege4lua_tointeger(__FUNCTION__,L,3),
					ege4lua_tointeger(__FUNCTION__,L,4));
	}
	else if(lua_type(L,6) == LUA_TNONE)
	{
		setviewport(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2),
					ege4lua_tointeger(__FUNCTION__,L,3),
					ege4lua_tointeger(__FUNCTION__,L,4),
					ege4lua_tointeger(__FUNCTION__,L,5));
	}
	else if(lua_type(L,6) == LUA_TLIGHTUSERDATA)
	{
		setviewport(ege4lua_tointeger(__FUNCTION__,L,1),
					ege4lua_tointeger(__FUNCTION__,L,2),
					ege4lua_tointeger(__FUNCTION__,L,3),
					ege4lua_tointeger(__FUNCTION__,L,4),
					ege4lua_tointeger(__FUNCTION__,L,5),
					static_cast<PIMAGE>(lua_touserdata(L,6)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}
	
	return 0;
}


int ege4lua_setvisualpage(lua_State* L)
{
	int page = ege4lua_tointeger(__FUNCTION__,L,1);
	if(page<0 || page>3)
	{
		lua_pushboolean(L,false);
	}
	else
	{
		setvisualpage(page);
		lua_pushboolean(L,true);
	}

	return 1;
}



int ege4lua_window_getviewport(lua_State* L)
{
	if(lua_type(L,1) != LUA_TNONE)
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}
	else
	{
		int pleft;
		int ptop;
		int pright;
		int pbottom;
		window_getviewport(&pleft,&ptop,&pright,&pbottom);
		lua_newtable(L);//创建一张表返回

		lua_pushstring(L,"left");
		lua_pushinteger(L,pleft);
		lua_settable(L,-3);

		lua_pushstring(L,"top");
		lua_pushinteger(L,ptop);
		lua_settable(L,-3);

		lua_pushstring(L,"right");
		lua_pushinteger(L,pright);
		lua_settable(L,-3);

		lua_pushstring(L,"bottom");
		lua_pushinteger(L,pbottom);
		lua_settable(L,-3);
	}

	return 1;
}


int ege4lua_window_setviewport(lua_State* L)
{
	window_setviewport(ege4lua_tointeger(__FUNCTION__,L,1),
						ege4lua_tointeger(__FUNCTION__,L,2),
						ege4lua_tointeger(__FUNCTION__,L,3),
						ege4lua_tointeger(__FUNCTION__,L,4));
	return 0;
}


/* 绘图参数(颜色、风格、当前点等)相关函数 */
int ege4lua_getred(lua_State* L)
{
	int rgb = ege4lua_tointeger(__FUNCTION__,L,1);
	lua_pushinteger(L,(rgb & 0xff0000)>>16);
	return 1;
}

int ege4lua_getgreen(lua_State* L)
{
	int rgb = ege4lua_tointeger(__FUNCTION__,L,1);
	lua_pushinteger(L,(rgb & 0x00ff00)>>8);
	return 1;
}

int ege4lua_getblue(lua_State* L)
{
	int rgb = ege4lua_tointeger(__FUNCTION__,L,1);
	lua_pushinteger(L,(rgb & 0x0000ff));
	return 1;
}

int ege4lua_getbkcolor(lua_State* L)
{
	if(lua_type(L,1) == LUA_TNONE)
	{
		lua_pushinteger(L,getbkcolor());
	}
	else if(lua_type(L,1) == LUA_TLIGHTUSERDATA)
	{
		lua_pushinteger(L,getbkcolor(static_cast<PIMAGE>(lua_touserdata(L,1))));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 1;
}

int ege4lua_getcolor(lua_State* L)
{

	if(lua_type(L,1) == LUA_TNONE)
	{
		lua_pushinteger(L,getcolor());
	}
	else if(lua_type(L,1) == LUA_TLIGHTUSERDATA)
	{
		lua_pushinteger(L,getcolor(static_cast<PIMAGE>(lua_touserdata(L,1))));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 1;
}

int ege4lua_getfillcolor(lua_State* L)
{
	if(lua_type(L,1) == LUA_TNONE)
	{
		lua_pushinteger(L,getfillcolor());
	}
	else if(lua_type(L,1) == LUA_TLIGHTUSERDATA)
	{
		lua_pushinteger(L,getfillcolor(static_cast<PIMAGE>(lua_touserdata(L,1))));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 1;
}

int ege4lua_hsl2rgb(lua_State* L)
{
	float HSL_H = luaL_checknumber(L,1);
	float HSL_S = luaL_checknumber(L,2);
	float HSL_L = luaL_checknumber(L,3);
	lua_pushinteger(L,hsl2rgb(HSL_H,HSL_S,HSL_L));

	return 1;
}

int ege4lua_hsv2rgb(lua_State* L)
{
	float HSV_H = luaL_checknumber(L,1);
	float HSV_S = luaL_checknumber(L,2);
	float HSV_V = luaL_checknumber(L,3);
	lua_pushinteger(L,hsl2rgb(HSV_H,HSV_S,HSV_V));

	return 1;
}

int ege4lua_rgb2gray(lua_State* L)
{
	lua_pushinteger(L,ege4lua_tointeger(__FUNCTION__,L,1));
	return 1;
}

int ege4lua_rgb2hsl(lua_State* L)
{
	float HSL_H;
	float HSL_S;
	float HSL_L;
	rgb2hsl(ege4lua_tointeger(__FUNCTION__,L,1),&HSL_H,&HSL_S,&HSL_L);
	lua_newtable(L);//创建一张表返回

	lua_pushstring(L,"H");
	lua_pushnumber(L,HSL_H);
	lua_settable(L,-3);

	lua_pushstring(L,"S");
	lua_pushnumber(L,HSL_S);
	lua_settable(L,-3);

	lua_pushstring(L,"L");
	lua_pushnumber(L,HSL_L);
	lua_settable(L,-3);

	return 1;

}

int ege4lua_rgb2hsv(lua_State* L)
{
	float HSV_H;
	float HSV_S;
	float HSV_V;
	rgb2hsl(ege4lua_tointeger(__FUNCTION__,L,1),&HSV_H,&HSV_S,&HSV_V);
	lua_newtable(L);//创建一张表返回

	lua_pushstring(L,"H");
	lua_pushnumber(L,HSV_H);
	lua_settable(L,-3);

	lua_pushstring(L,"S");
	lua_pushnumber(L,HSV_S);
	lua_settable(L,-3);

	lua_pushstring(L,"L");
	lua_pushnumber(L,HSV_V);
	lua_settable(L,-3);

	return 1;

}


int ege4lua_setbkcolor(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setbkcolor(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setbkcolor(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_setclrcolor(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setbkcolor_f(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setbkcolor_f(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_setbkmode(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setbkmode(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setbkmode(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_setcolor(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setcolor(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setcolor(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_setfillcolor(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setfillcolor(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setfillcolor(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_setfontbkcolor(lua_State* L)
{
	if(lua_type(L,2) == LUA_TNONE)
	{
		setfontbkcolor(ege4lua_tointeger(__FUNCTION__,L,1));
	}
	else if(lua_type(L,2) == LUA_TLIGHTUSERDATA)
	{
		setfontbkcolor(ege4lua_tointeger(__FUNCTION__,L,1),static_cast<PIMAGE>(lua_touserdata(L,2)));
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_setfillstyle(lua_State* L)
{
	int pattern = ege4lua_tointeger(__FUNCTION__,L,1);
	color_t color = ege4lua_tointeger(__FUNCTION__,L,2);

	if(lua_isnone(L,3))
	{
		setfillstyle(pattern,color);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		setfillstyle(pattern,color,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_setlinestyle(lua_State* L)
{
	int linestyle = ege4lua_tointeger(__FUNCTION__,L,1);
	int thickness = ege4lua_tointeger(__FUNCTION__,L,2);
	if(lua_isnone(L,3))
	{
		setlinestyle(linestyle,0,thickness);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		setlinestyle(linestyle,0,thickness,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_setlinewidth(lua_State* L)
{
	int thickness = ege4lua_tointeger(__FUNCTION__,L,1);
	if(lua_isnone(L,2))
	{
		setlinewidth(thickness);
	}
	else if(lua_isuserdata(L,2))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,2));
		setlinewidth(thickness,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_getheight(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,getheight());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,getheight(pimg));
	}

	return 1;
}



int ege4lua_getwidth(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,getwidth());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,getwidth(pimg));
	}

	return 1;
}


int ege4lua_getsize(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,getheight());
		lua_pushinteger(L,getwidth());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,getheight(pimg));
		lua_pushinteger(L,getwidth(pimg));
	}

	return 2;
}



int ege4lua_getx(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,getx());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,getx(pimg));
	}

	return 1;
}


int ege4lua_gety(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,gety());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,gety(pimg));
	}

	return 1;
}


int ege4lua_getposition(lua_State* L)
{
	if(lua_isnone(L,1))
	{
		lua_pushinteger(L,getx());
		lua_pushinteger(L,gety());
	}
	else if(lua_isuserdata(L,1))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
		lua_pushinteger(L,getx(pimg));
		lua_pushinteger(L,gety(pimg));
	}

	return 2;
}

int ege4lua_getpixelcolor(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	if(lua_isnone(L,3))
	{
		lua_pushinteger(L,getpixel(x,y));
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		lua_pushinteger(L,getpixel(x,y,pimg));
	}

	return 1;
}

int ege4lua_moverel(lua_State* L)
{
	int dx = ege4lua_tointeger(__FUNCTION__,L,1);//位移量
	int dy = ege4lua_tointeger(__FUNCTION__,L,2);//位移量

	if(lua_isnone(L,3))
	{
		moverel(dx,dy);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		moverel(dx,dy,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_moveto(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);//绝对左边
	int y = ege4lua_tointeger(__FUNCTION__,L,2);//绝对坐标

	if(lua_isnone(L,3))
	{
		moveto(x,y);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		moveto(x,y,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


/* 绘制图形相关函数 */
int ege4lua_arc(lua_State* L)//画圆弧
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int stangle = ege4lua_tointeger(__FUNCTION__,L,3);
	int endangle = ege4lua_tointeger(__FUNCTION__,L,4);
	int radius = ege4lua_tointeger(__FUNCTION__,L,5);
	
	if(lua_type(L,6) == LUA_TNONE)
	{
		arc(x,y,stangle,endangle,radius);
	}
	else if(lua_type(L,6) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,6));
		arc(x,y,stangle,endangle,radius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_bar(lua_State* L)//无边框重填矩形
{
	int left = ege4lua_tointeger(__FUNCTION__,L,1);
	int top = ege4lua_tointeger(__FUNCTION__,L,2);
	int right = ege4lua_tointeger(__FUNCTION__,L,3); 
	int bottom = ege4lua_tointeger(__FUNCTION__,L,4);
	
	if(lua_type(L,5) == LUA_TNONE)
	{
		bar(left,top,right,bottom);
	}
	else if(lua_type(L,5) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,5));
		bar(left,top,right,bottom,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_bar3d(lua_State* L)//长方体
{
	int left = ege4lua_tointeger(__FUNCTION__,L,1);
	int top = ege4lua_tointeger(__FUNCTION__,L,2);
	int right = ege4lua_tointeger(__FUNCTION__,L,3);
	int bottom = ege4lua_tointeger(__FUNCTION__,L,4);
	int depth = ege4lua_tointeger(__FUNCTION__,L,5);
	bool topflag = lua_toboolean(L,6);
	if(lua_type(L,7) == LUA_TNONE)
	{
		bar3d(left,top,right,bottom,depth,topflag);
	}
	else if(lua_type(L,7) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,7));
		bar3d(left,top,right,bottom,depth,topflag,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_circle(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int radius = ege4lua_tointeger(__FUNCTION__,L,3);

	if(lua_type(L,4) == LUA_TNONE)
	{
		circle(x,y,radius);
	}
	else if(lua_type(L,4) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		circle(x,y,radius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_drawbezier(lua_State* L)
{
	if(!lua_istable(L,1))
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	int length = luaL_len(L,1);

	if(length % 2 != 0 || length/2%3 != 1)//由于表示坐标，应当为2的整数倍,drawbezier绘图时点数应除以3余1
	{
		ege4lua_warning(__FUNCTION__,"length of table is ill-suited.\n");
	}

	int polypoints[length];
	ege4lua_getintarray(__FUNCTION__,L,polypoints);

	if(lua_isnone(L,2))
	{
		drawbezier(length/2,polypoints);
	}
	else if(lua_isuserdata(L,2))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,2));
		drawbezier(length/2,polypoints,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_drawlines(lua_State* L)
{
	if(!lua_istable(L,1))
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	int length = luaL_len(L,1);
	if(length%4 != 0)
	{
		ege4lua_warning(__FUNCTION__,"length of table is ill-suited.\n");
	}

	int polypoints[length];
	ege4lua_getintarray(__FUNCTION__,L,polypoints);

	if(lua_isnone(L,2))
	{
		drawlines(length/4,polypoints);
	}
	else if(lua_isuserdata(L,2))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,2));
		drawlines(length/2,polypoints,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_drawpoly(lua_State* L)
{
	if(!lua_istable(L,1))
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	int length = luaL_len(L,1);
	if(length%2 != 0)
	{
		ege4lua_warning(__FUNCTION__,"length of table is ill-suited.\n");
	}

	int polypoints[length];
	ege4lua_getintarray(__FUNCTION__,L,polypoints);

	if(lua_isnone(L,2))
	{
		drawpoly(length/2,polypoints);
	}
	else if(lua_isuserdata(L,2))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,2));
		drawpoly(length/2,polypoints,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_ellipse(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int stangle = ege4lua_tointeger(__FUNCTION__,L,3);
	int endangle = ege4lua_tointeger(__FUNCTION__,L,4);
	int xradius = ege4lua_tointeger(__FUNCTION__,L,5);
	int yradius = ege4lua_tointeger(__FUNCTION__,L,6);

	if(lua_isnone(L,7))
	{
		ellipse(x,y,stangle,endangle,xradius,yradius);
	}
	else if(lua_isuserdata(L,7))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,7));
		ellipse(x,y,stangle,endangle,xradius,yradius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_fillellipse(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int xradius = ege4lua_tointeger(__FUNCTION__,L,3);
	int yradius = ege4lua_tointeger(__FUNCTION__,L,4);

	if(lua_isnone(L,5))
	{
		fillellipse(x,y,xradius,yradius);
	}
	else if(lua_isuserdata(L,5))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,5));
		fillellipse(x,y,xradius,yradius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_fillpoly(lua_State* L)
{
	if(!lua_istable(L,1))
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	int length = luaL_len(L,1);
	if(length%2 != 0)
	{
		ege4lua_warning(__FUNCTION__,"length of table is ill-suited.\n");
	}

	int polypoints[length];
	ege4lua_getintarray(__FUNCTION__,L,polypoints);
	if(lua_isnone(L,2))
	{
		fillpoly(length/2,polypoints);
	}
	else if(lua_isuserdata(L,2))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,2));
		fillpoly(length/2,polypoints,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_floodfill(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int border = ege4lua_tointeger(__FUNCTION__,L,3);

	if(lua_isnone(L,4))
	{
		floodfill(x,y,border);
	}
	else if(lua_isuserdata(L,4))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,4));
		floodfill(x,y,border,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_floodfillsurface(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int areacolor = ege4lua_tointeger(__FUNCTION__,L,3);

	if(lua_isnone(L,4))
	{
		floodfillsurface(x,y,areacolor);
	}
	else if(lua_isuserdata(L,4))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,4));
		floodfillsurface(x,y,areacolor,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}
	return 0;
}


int ege4lua_line(lua_State* L)
{
	int x1 = ege4lua_tointeger(__FUNCTION__,L,1);
	int y1 = ege4lua_tointeger(__FUNCTION__,L,2);
	int x2 = ege4lua_tointeger(__FUNCTION__,L,3);
	int y2 = ege4lua_tointeger(__FUNCTION__,L,4);

	if(lua_isnone(L,5))
	{
		line(x1,y1,x2,y2);
	}
	else if(lua_isuserdata(L,5))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,5));
		line(x1,y1,x2,y2,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_linerel(lua_State* L)
{
	int dx = ege4lua_tointeger(__FUNCTION__,L,1);//位移量
	int dy = ege4lua_tointeger(__FUNCTION__,L,2);//位移量

	if(lua_isnone(L,3))
	{
		linerel(dx,dy);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		linerel(dx,dy,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_lineto(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);//绝对左边
	int y = ege4lua_tointeger(__FUNCTION__,L,2);//绝对坐标

	if(lua_isnone(L,3))
	{
		lineto(x,y);
	}
	else if(lua_isuserdata(L,3))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,3));
		lineto(x,y,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_sector(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int stangle = ege4lua_tointeger(__FUNCTION__,L,3);
	int endangle = ege4lua_tointeger(__FUNCTION__,L,4);
	int radius = ege4lua_tointeger(__FUNCTION__,L,5);
	
	if(lua_type(L,6) == LUA_TNONE)
	{
		pieslice(x,y,stangle,endangle,radius);
	}
	else if(lua_type(L,6) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,6));
		pieslice(x,y,stangle,endangle,radius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_ellipticsector(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int stangle = ege4lua_tointeger(__FUNCTION__,L,3);
	int endangle = ege4lua_tointeger(__FUNCTION__,L,4);
	int xradius = ege4lua_tointeger(__FUNCTION__,L,5);
	int yradius = ege4lua_tointeger(__FUNCTION__,L,6);
	
	if(lua_type(L,7) == LUA_TNONE)
	{
		sector(x,y,stangle,endangle,xradius,yradius);
	}
	else if(lua_type(L,7) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,6));
		sector(x,y,stangle,endangle,xradius,yradius,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_rectangle(lua_State* L)
{
	int left = ege4lua_tointeger(__FUNCTION__,L,1);
	int top = ege4lua_tointeger(__FUNCTION__,L,2);
	int right = ege4lua_tointeger(__FUNCTION__,L,3); 
	int bottom = ege4lua_tointeger(__FUNCTION__,L,4);
	
	if(lua_type(L,5) == LUA_TNONE)
	{
		rectangle(left,top,right,bottom);
	}
	else if(lua_type(L,5) == LUA_TLIGHTUSERDATA)
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,5));
		rectangle(left,top,right,bottom,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

int ege4lua_putpixel(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	color_t color = ege4lua_tointeger(__FUNCTION__,L,3);

	if(lua_isnone(L,4))
	{
		putpixel(x,y,color);
	}
	else if(lua_isuserdata(L,4))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,4));
		putpixel(x,y,color,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}

/* 文字输出相关函数 */
int ege4lua_setfont(lua_State* L)
{
	int height = ege4lua_tointeger(__FUNCTION__,L,1);//高度
	int width = ege4lua_tointeger(__FUNCTION__,L,2);//宽度
	const char* font = luaL_checkstring(L,3);//字体
	int weight = lua_tointeger(L,4);//粗细
	bool ifItalic = lua_toboolean(L,5);//是否斜体
	bool ifUnderline = lua_toboolean(L,6);//是否有下划线
	bool ifdeleteline = lua_toboolean(L,7);//是否有删除线


	if(lua_isnone(L,8))
	{
		setfont(height,width,font,0,0,weight,ifItalic,ifUnderline,ifdeleteline);
	}
	else if(lua_isuserdata(L,4))
	{
		PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,4));
		setfont(height,width,font,0,0,weight,ifItalic,ifUnderline,ifdeleteline,pimg);
	}
	else
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	return 0;
}


int ege4lua_xyprint(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	const char* str = luaL_checkstring(L,3);
	xyprintf(x,y,str);

	return 0;
}


/* 图像处理相关函数 */
int ege4lua_putimage(lua_State* L)
{
	if(!lua_isuserdata(L,1))
	{
		ege4lua_error(__FUNCTION__,L,"parameters is incorrect.\n");
	}

	PIMAGE pimg = static_cast<PIMAGE>(lua_touserdata(L,1));
	int x = ege4lua_tointeger(__FUNCTION__,L,2);
	int y = ege4lua_tointeger(__FUNCTION__,L,3);
	putimage(x,y,pimg);

	return 0;
}


int ege4lua_getimage(lua_State* L)
{
	const char* file = luaL_checkstring(L,1);
	int width = lua_tointeger(L,2);
	int height = lua_tointeger(L,3);
	PIMAGE pimg = newimage(width,height);
	getimage(pimg,file,width,height);
	lua_pushlightuserdata(L,pimg);

	return 1;
}


int ege4lua_screenshot(lua_State* L)
{
	int x = ege4lua_tointeger(__FUNCTION__,L,1);
	int y = ege4lua_tointeger(__FUNCTION__,L,2);
	int width = ege4lua_tointeger(__FUNCTION__,L,3);
	int height = ege4lua_tointeger(__FUNCTION__,L,4);
	PIMAGE pimg = newimage(width,height);
	getimage(pimg,x,y,width,height);

	lua_pushlightuserdata(L,pimg);

	return 1;
}


/* 键盘鼠标输入函数 */
int ege4lua_getch(lua_State* L)
{
	int key = getch();
	lua_pushinteger(L,key);
	return 1;
}

int ege4lua_keystate(lua_State* L)
{
	int key = ege4lua_tointeger(__FUNCTION__,L,1);
	lua_pushboolean(L,keystate(key));

	return 1;
}

/* 时间函数 */
int ege4lua_delay_fps(lua_State* L)
{
	int fps = ege4lua_tointeger(__FUNCTION__,L,1);
	delay_fps(fps);
	return 0;
}

/* 数学函数 */


/* 随机函数 */


/* 其它函数 */
int ege4lua_getfps(lua_State* L)
{
	lua_pushnumber(L,getfps());

	return 1;
}

