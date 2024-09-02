#include <NanoCanvas.h>

#ifdef NANOVG_GLEW
#	include <GL/glew.h>
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"


int main()
{
	
	NVGcontext * nvgCtx = nvgCreateGL3(NVG_ANTIALIAS | NVG_DEBUG |NVG_STENCIL_STROKES);
	NanoCanvas::Canvas canvas(nvgCtx,800 ,600);
	if(canvas.valid()) {

		while ( 1 )
		{
			canvas.begineFrame(800,600);
			// Everything is OK
			canvas.clearColor(NanoCanvas::Colors::DarkOliveGreen); //Clear canvas with color

			// Draw a rounded rectangle
			canvas.beginPath()
				.roundedRect(50,50,100,100,10)
				.fillStyle(NanoCanvas::Colors::Salmon)
				.fill();

			// Draw styled text
			NanoCanvas::TextStyle textStyle;
			NanoCanvas::Font font;
			textStyle.size = 36.0f;
			textStyle.color = NanoCanvas::Colors::White;
			textStyle.face = font.face;
			canvas.fillStyle(textStyle)
				.beginPath()
				.fillText("Hello Canvas",30,190);
			canvas.endFrame();
		}

		
	}

	

	return 0;
}