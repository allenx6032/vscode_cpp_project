////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_Renderer.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Element.h"
#include "Gugu_Window.h"
#include "Gugu_Camera.h"
#include "Gugu_Scene.h"

#include <SFML_RenderTarget.hpp>
#include <SFML_RenderWindow.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

void Renderer::RenderScene(FrameInfos* frameInfos, Window* window, Scene* scene, Camera* camera)
{
    DefaultRenderScene(frameInfos, window, scene, camera);
}

void Renderer::RenderWindow(FrameInfos* frameInfos, Window* window, Camera* camera)
{
    DefaultRenderWindow(frameInfos, window, camera);
}

void Renderer::DefaultRenderScene(FrameInfos* frameInfos, Window* window, Scene* scene, Camera* camera)
{
    if (!window || !scene)
        return;

    RenderPass renderPass;
    renderPass.pass = GUGU_RENDERPASS_DEFAULT;
    renderPass.target = window->GetSFRenderWindow();
    renderPass.frameInfos = frameInfos;

    RenderElementHierarchy(renderPass, scene->GetRootNode(), camera);
}

void Renderer::DefaultRenderWindow(FrameInfos* frameInfos, Window* window, Camera* camera)
{
    if (!window)
        return;

    RenderPass renderPass;
    renderPass.pass = GUGU_RENDERPASS_DEFAULT;
    renderPass.target = window->GetSFRenderWindow();
    renderPass.frameInfos = frameInfos;

    RenderElementHierarchy(renderPass, window->GetUINode(), camera);
    RenderElementHierarchy(renderPass, window->GetMouseNode(), camera);
}

sf::FloatRect Renderer::ComputeViewport(const sf::View& view)
{
    sf::FloatRect viewport;

    // Pre-compute viewport's rect (real size, not a [0, 1] range) (will be used by Elements to check if they should be drawn or not)
    viewport.left = view.getCenter().x - view.getSize().x / 2.f;
    viewport.top = view.getCenter().y - view.getSize().y / 2.f;
    viewport.width = view.getSize().x;
    viewport.height = view.getSize().y;

    return viewport;
}

void Renderer::RenderElementHierarchy(RenderPass& renderPass, Element* root, Camera* camera)
{
    if (!root)
        return;

    // Apply Camera View if needed.
    sf::View backupView;

    if (camera)
    {
        backupView = renderPass.target->getView();
        renderPass.target->setView(camera->GetSFView());
    }

    // Compute Viewport.
    renderPass.rectViewport = ComputeViewport(renderPass.target->getView());

#if 0
    // Debug off-screen rendering.
    renderPass.rectViewport.left += 50.f;
    renderPass.rectViewport.top += 50.f;
    renderPass.rectViewport.width -= 100.f;
    renderPass.rectViewport.height -= 100.f;
#endif

    // Render hierarchy.
    root->Render(renderPass, sf::Transform());

    // Restore View if needed.
    if (camera)
    {
        renderPass.target->setView(backupView);
    }
}

void DefaultRenderer::RenderScene(FrameInfos* frameInfos, Window* window, Scene* scene, Camera* camera)
{
    DefaultRenderScene(frameInfos, window, scene, camera);
}

void DefaultRenderer::RenderWindow(FrameInfos* frameInfos, Window* window, Camera* camera)
{
    DefaultRenderWindow(frameInfos, window, camera);
}

}   // namespace gugu
