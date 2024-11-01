#include "e2d_scene.h"
#include "e2d_camera.h"
#include "e2d_scene_mgr.h"
#include "e2d_pos.h"
#include "e2d_input.h"
#include "e2d_graphics_mgr.h"
#include "e2d_render_batch.h"
#include "e2d_transform_component.h"

/* Easy2D */
using namespace Easy2D;

Scene::Scene(const String& name) : Entity(name)
{
}

Scene::~Scene()
{
}

void Scene::destroy()
{
    SceneManager::instance()->removeScene(mGUID);
}

bool Scene::setup()
{
    if (!Entity::setup())
    {
        LOG_WARN << ("Scene::setup: Entity setup failed!");
        return false;
    }
    mDefCamera = std::make_shared<Camera>("def_camera");
    if (!mDefCamera->setup())
    {
        LOG_WARN << ("Entity::setup: create Default Camera failed!");
        return false;
    }
    addChild(mDefCamera);
    setActiveCamera(mDefCamera);
//    InputSystem::instance()->addInputSink(std::dynamic_pointer_cast<InputSink>(shared_from_this()));
    return true;
}

void Scene::onActivate()
{

}

void Scene::onDeactivate()
{
    for (auto entity : mChildrens)
    {
        entity->reset();
    }
}

void Scene::setActiveCamera(SPtr<Camera> pCamera)
{
    if (!pCamera || mActiveCamera == pCamera)
    {
        return;
    }
    if (mActiveCamera != nullptr)
    {
        mActiveCamera->deactivate();
    }
    mActiveCamera = pCamera;
    mActiveCamera->activate();
}

SPtr<Camera> Scene::getActiveCamera() const
{
    return mActiveCamera;
}
