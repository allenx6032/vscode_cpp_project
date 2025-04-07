#include <plib_ul.h>
#include <plib_js.h>
#include <plib_sg.h>
#include <plib_fnt.h>
#include <plib_pu.h>
#include <plib_ssg.h>
#include <plib_pw.h>
#include <plib_puAux.h>
#include <plib_psl.h>
#include <plib_sl.h>
#include <plib_net.h>
#include <plib_ssgAux.h>

void renderScene(){
	PLIB::Graphics gfx;
	gfx.clear();
	gfx.display();
}

int main(){
	PLIB::Window win;
	win.create("PLIB Demo", 800, 600);
	while(!win.isCLosed()){
		win.clear();
		renderScene();
		win.update();
	}
	return 0;
}