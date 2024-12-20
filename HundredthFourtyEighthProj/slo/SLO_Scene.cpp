#include "SLO_Scene.h"
#include "SLO_SDLWindow.h"
#include "SLO_GLUCompatible.h"
#include "SLO_Application.h"
#include "SLO_Resources.h"
#include "SLO_TaskManager.h"
#include <SDL_log.h>
#include <SDL_opengl.h>

Scene::Scene(uint32_t windowId)
	: nextScene_()
	, window_(SDL_::Window::getWindow(windowId))
	, manager_(nullptr)
	, isFinished_(false)
{
}

void Scene::setNextScene(std::shared_ptr<Scene> scene)
{
	if (!scene) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "next scene is null!");
	}
	else {
		nextScene_ = scene;
	}
}

void Scene::onAddJoystick(int index)
{
	// nop
}

void Scene::update()
{
	window_->update();
}

void Scene::clear()
{
	::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::swap()
{
	window_->swap();
}

bool Scene::onIdle(uint32_t tick)
{
	return !manager_->compute(tick);
}

void Scene::onCreate()
{
}

void Scene::setTaskManager(TaskManager &manager) {
	manager_ = &manager;
}

void Scene::unsetTaskManager() {
	manager_->clear();
	manager_ = nullptr;
}

void Scene::registerTask(int id, std::shared_ptr<Task> &&task)
{
	manager_->registerTask(id, std::move(task));
}

void Scene::unregisterTask(int id, bool isFinishAction)
{
	manager_->unregisterTask(id, isFinishAction);
}

void Scene::onDestroy()
{
}

void Scene::quit()
{
	Application::quit();
}

void Scene::calcViewPort(int ww, int wh, int sw, int sh)
{
	if (static_cast<double>(ww)/static_cast<double>(wh) != static_cast<int>(sw)/static_cast<double>(sh)) {
		const auto waspect = static_cast<double>(ww)/static_cast<double>(sw);
		const auto haspect = static_cast<double>(wh)/static_cast<double>(sh);
		if(waspect > haspect) {
			auto pw = sw * haspect;
			auto ph = wh;
			::glViewport((ww-pw)/2, 0, pw, ph);
		}
		else {
			auto ph = sh * waspect;
			auto pw = ww;
			::glViewport(0, (wh-ph)/2, pw, ph);
		}
	}
	else {
		::glViewport(0, 0, ww, wh);
	}
}

void Scene::Enter2DMode()
{
	auto &res = Resources::instance();
	const auto ww = res.getWindowWidth();
	const auto wh = res.getWindowHeight();
	const auto sw = res.getScreenWidth();
	const auto sh = res.getScreenHeight();

	::glPushAttrib(GL_ENABLE_BIT);
	::glDisable(GL_DEPTH_TEST);
	::glDisable(GL_CULL_FACE);
	::glEnable(GL_TEXTURE_2D);

	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();
	::glOrtho(0.0, sw, sh, 0.0, -10000.0, 10000.0);
	calcViewPort(ww, wh, sw, sh);

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();
}

void Scene::Leave2DMode()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
}

void Scene::Enter3DMode()
{
	auto &res = Resources::instance();
	const auto ww = res.getWindowWidth();
	const auto wh = res.getWindowHeight();
	const auto sw = res.getScreenWidth();
	const auto sh = res.getScreenHeight();

	::glPushAttrib(GL_ENABLE_BIT);
	::glShadeModel(GL_SMOOTH);
	::glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	::glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	::glEnable(GL_MULTISAMPLE);
	::glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	::glSampleCoverage(0.5,GL_FALSE);

#if 1
	::glCullFace(GL_BACK);
	::glFrontFace(GL_CCW);
	::glEnable(GL_CULL_FACE);
#else
	::glDisable(GL_CULL_FACE);
#endif

	::glEnable(GL_DEPTH_TEST);
	::glClearDepth(1.0);

	::glEnable(GL_TEXTURE_2D);
	::glClearColor(0.0, 0.0, 0.0, 0.0);
	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();
	const double fovy = res.getViewAngle();
	const double znear = 1.0;
	const double zfar = 10000.0;
	::gluPerspectiveCompatible(fovy , static_cast<double>(sw)/static_cast<double>(sh) , znear, zfar);
	calcViewPort(ww, wh, sw, sh);

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();
}

void Scene::Leave3DMode()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
}

