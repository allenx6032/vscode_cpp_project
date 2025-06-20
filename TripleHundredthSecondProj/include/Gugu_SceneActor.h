#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_DeltaTime.h"

#include <vector>

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class Scene;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class SceneActor
{
    friend class Scene;

public:

    SceneActor();
    virtual ~SceneActor();

    virtual void Step(const DeltaTime& dt);
    virtual void Update(const DeltaTime& dt);
    virtual void LateUpdate(const DeltaTime& dt);
    virtual void UpdateImGui(const DeltaTime& dt);

    void RemoveFromScene();

    Scene* GetScene() const;

protected:

    virtual void OnAddedToScene() {}
    virtual void OnRemovedFromScene() {}

protected:

    Scene* m_scene;
};

}   // namespace gugu
