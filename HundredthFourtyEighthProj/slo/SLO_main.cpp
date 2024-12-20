#include "SLO_Application.h"
#include "SLO_SDLWindow.h"
#include "SLO_Resources.h"
#include "SLO_OpeningScene.h"
#include <SDL_log.h>
#include <SDL_opengl.h>
#include <memory>


namespace
{
void setup_opengl()
{
	::SDL_GL_SetAttribute
	(
	    SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
	);
	::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	::SDL_GL_SetSwapInterval(1);

	::glShadeModel(GL_SMOOTH);
	::glEnable(GL_TEXTURE_2D);
	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glClearColor(0.0, 0.0, 0.0, 0.0);

	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	::glCullFace(GL_BACK);
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);

	::glEnable(GL_DEPTH_TEST);
	::glClearDepth(1.0);

   ::glMatrixMode(GL_PROJECTION);
   ::glLoadIdentity();
	// resize(width, height, width, height, 45.0, 1.0, 100000.0);

	// const GLfloat lightPos[] = {0.0, 0.0, 1000, 1.0};
	// const GLfloat lightDirection[] = {0.0, 0.0, -1.0};
	// const GLfloat lightDeffuse[] = {0.8, 0.8, 0.8, 0.9};
	// const GLfloat lightAmbient[] = {1, 1, 1, 0.2};
	// const GLfloat lightSpecular[] = {1, 1, 1, 0.1};

	// ::glEnable(GL_LIGHTING);
	// ::glEnable(GL_LIGHT0);
	// ::glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	// ::glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	// ::glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDeffuse);
	// ::glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	// ::glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	// GL_AMBIENT(0.2 , 0.2 , 0.2 , 1)
	// const GLfloat MaterialAmbient[] = {0.5, 0.5, 0.5, 0.2};
	// GL_DIFFUSE(0.8 , 0.8 , 0.8 , 1)
	// const GLfloat MaterialDeffuse[] = {0.5, 0.5, 0.5, 0.8};
	// GL_SPECULAR(0 , 0 , 0, 1)
	// const GLfloat MaterialSpecular[] = {0.3, 0.3, 0.3, 0.1};
	// GL_EMISSION(0 , 0 , 0 , 1)
	// const GLfloat MaterialEmission[] = {1.0, 1.0, 1.0, 0.1};
	// GL_SHININESS(0)
	// const GLfloat MaterialShininess = 1.0;
	// ::glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	// ::glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDeffuse);
	// ::glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
	// ::glMaterialfv(GL_FRONT, GL_EMISSION, MaterialEmission;
	// ::glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
}

} // nameless namespace

int main(int argc, char *argv[])
{
	//const std::vector<std::string> args(argv, argv+argc);

	Application app(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
	if(!app.isApplication()){
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Init error. %s", SDL_GetError());
		return 1;
	}

	Resources &res = Resources::instance();

	const int width = res.getWindowWidth();
	const int height = res.getWindowHeight();

	auto mainWindow = std::make_shared<SDL_::Window>("main", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	SDL_::Window::registWindow(mainWindow);
	auto size = mainWindow->getSize();
	res.setWindowWidth(size.get_width());
	res.setWindowHeight(size.get_height());

	setup_opengl();

	return app.run(std::make_shared<OpeningScene>(mainWindow->getWindowId()));
}
