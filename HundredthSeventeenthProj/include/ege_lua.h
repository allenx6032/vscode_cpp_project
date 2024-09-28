#ifndef EGE4LUA_H
#define EGE4LUA_H


/* 绘图环境相关函数 */
int ege4lua_cleardevice(lua_State* L);
int ege4lua_clearviewport(lua_State* L);
int ege4lua_closegraph(lua_State* L);
int ege4lua_gettarget(lua_State* L);
int ege4lua_getviewport(lua_State* L);
int ege4lua_initgraph(lua_State* L);
int ege4lua_isrun(lua_State* L);
int ege4lua_setactivepage(lua_State* L);
int ege4lua_setcaption(lua_State* L);
int ege4lua_setinitmode(lua_State* L);
int ege4lua_setrendermode(lua_State* L);
int ege4lua_settarget(lua_State* L);
int ege4lua_setviewport(lua_State* L);
int ege4lua_setvisualpage(lua_State* L);
int ege4lua_window_getviewport(lua_State* L);
int ege4lua_window_setviewport(lua_State* L);

/* 绘图参数(颜色、风格、当前点等)相关函数 */
int ege4lua_getred(lua_State* L);
int ege4lua_getgreen(lua_State* L);
int ege4lua_getblue(lua_State* L);
int ege4lua_getbkcolor(lua_State* L);
int ege4lua_getcolor(lua_State* L);
int ege4lua_getfillcolor(lua_State* L);
int ege4lua_hsl2rgb(lua_State* L);
int ege4lua_hsv2rgb(lua_State* L);
int ege4lua_rgb2gray(lua_State* L);
int ege4lua_rgb2hsl(lua_State* L);
int ege4lua_rgb2hsv(lua_State* L);
int ege4lua_setbkcolor(lua_State* L);
int ege4lua_setclrcolor(lua_State* L);
int ege4lua_setbkmode(lua_State* L);
int ege4lua_setcolor(lua_State* L);
int ege4lua_setfillcolor(lua_State* L);
int ege4lua_setfontbkcolor(lua_State* L);
int ege4lua_moverel(lua_State* L);
int ege4lua_moveto(lua_State* L);
int ege4lua_setfillstyle(lua_State* L);
int ege4lua_setlinestyle(lua_State* L);
int ege4lua_setlinewidth(lua_State* L);
int ege4lua_getfillcolor(lua_State* L);
int ege4lua_getheight(lua_State* L);
int ege4lua_getwidth(lua_State* L);
int ege4lua_getsize(lua_State* L);
int ege4lua_getx(lua_State* L);
int ege4lua_gety(lua_State* L);
int ege4lua_getposition(lua_State* L);
int ege4lua_getpixelcolor(lua_State* L);

/* 绘制图形相关函数 */
int ege4lua_arc(lua_State* L);//画圆弧
int ege4lua_bar(lua_State* L);//画无边框重填矩形
int ege4lua_bar3d(lua_State* L);//画长方体
int ege4lua_circle(lua_State* L);//画圆
int ege4lua_drawbezier(lua_State* L);//画贝赛尔曲线
int ege4lua_drawlines(lua_State* L);//画不连续的几条线段
int ege4lua_drawpoly(lua_State* L);//画多连续线段
int ege4lua_ellipse(lua_State* L);//画椭圆弧
int ege4lua_fillellipse(lua_State* L);//画充填椭圆
int ege4lua_fillpoly(lua_State* L);//画重填多边形
int ege4lua_floodfill(lua_State* L);//区域充填
int ege4lua_floodfillsurface(lua_State* L);//区域充填
int ege4lua_line(lua_State* L);//画线
int ege4lua_linerel(lua_State* L);//从当前点画线(相对位移)
int ege4lua_lineto(lua_State* L);//从当前点画线(绝对坐标)
int ege4lua_sector(lua_State* L);//画扇形
int ege4lua_ellipticsector(lua_State* L);//画椭圆扇形
int ege4lua_rectangle(lua_State* L);//画空心矩形
int ege4lua_putpixel(lua_State* L);//画点


/* 文字输出相关函数 */
int ege4lua_setfont(lua_State* L);
int ege4lua_xyprint(lua_State* L);

/* 图像处理相关函数 */
int ege4lua_putimage(lua_State* L);
int ege4lua_getimage(lua_State* L);
int ege4lua_screenshot(lua_State* L);

/* 键盘鼠标输入函数 */
int ege4lua_getch(lua_State* L);
int ege4lua_keystate(lua_State* L);

/* 时间函数 */
int ege4lua_delay_fps(lua_State* L);

/* 其它函数 */
int ege4lua_getfps(lua_State* L);

#endif
