////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_Camera.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Window.h"
#include "Gugu_Scene.h"
#include "Gugu_Renderer.h"
#include "Gugu_Element.h"
#include "Gugu_MathUtility.h"

#include <SFML_RenderWindow.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
Camera::Camera()
{
    m_targetPosition = Vector2f(0.f, 0.f);
    m_size = Vector2f(0.f, 0.f);
    m_zoomMultiplier = 1.f;
    m_centerOnTarget = false;

    m_window = nullptr;
    m_scene = nullptr;
}

Camera::~Camera()
{
}

void Camera::SetWindow(Window* _pWindow)
{
    m_window = _pWindow;
}

Window* Camera::GetWindow() const
{
    return m_window;
}

void Camera::SetScene(Scene* scene)
{
    m_scene = scene;
}

Scene* Camera::GetScene() const
{
    return m_scene;
}

void Camera::SetSFView(const sf::View& _kView)
{
    m_sfView = _kView;
}

const sf::View& Camera::GetSFView() const
{
    return m_sfView;
}

void Camera::SetSize(float _fSizeX, float _fSizeY)
{
    SetSize(Vector2f(_fSizeX, _fSizeY));
}

void Camera::SetSize(Vector2f _kSize)
{
    m_size = _kSize;
    ComputeViewSize();
}

Vector2f Camera::GetSize() const
{
    return m_sfView.getSize();
}

void Camera::SetZoom(float zoom)
{
    m_zoomMultiplier = 1.f / zoom;
    ComputeViewSize();
}

float Camera::GetZoom() const
{
    return 1.f / m_zoomMultiplier;
}

void Camera::SetTarget(float _fTargetX, float _fTargetY)
{
    SetTarget(Vector2f(_fTargetX, _fTargetY));
}

void Camera::SetTarget(Vector2f _kTarget)
{
    m_targetPosition = _kTarget;
    ComputeViewCenter();
}

Vector2f Camera::GetTarget() const
{
    return m_targetPosition;
}

void Camera::SetCenterOnTarget(bool _bCenterOnTarget)
{
    m_centerOnTarget = _bCenterOnTarget;
    ComputeViewCenter();
}

bool Camera::IsCenterOnTarget() const
{
    return m_centerOnTarget;
}

void Camera::SetViewport(const sf::FloatRect& _kViewport)
{
    m_sfView.setViewport(_kViewport);
    ComputeViewSize();
}

void Camera::RecomputeSizeFromWindow()
{
    if (m_window)
    {
        SetSize(Vector2f(m_window->GetSize()));
    }
}

void Camera::ComputeViewSize()
{
    const sf::FloatRect& kViewport = m_sfView.getViewport();

    Vector2f adjustedSize(RoundFloor(m_size.x * kViewport.width * m_zoomMultiplier), RoundFloor(m_size.y * kViewport.height * m_zoomMultiplier));
    m_sfView.setSize(adjustedSize);

    ComputeViewCenter();
}

void Camera::ComputeViewCenter()
{
    if (m_centerOnTarget)
    {
        Vector2f viewSize = m_sfView.getSize();
        Vector2f adjustedTargetPosition(RoundFloor(m_targetPosition.x), RoundFloor(m_targetPosition.y));

        Vector2f offset(
            (int)viewSize.x % 2 == 1 ? 0.5f : 0.f,
            (int)viewSize.y % 2 == 1 ? 0.5f : 0.f
        );

        m_sfView.setCenter(adjustedTargetPosition + offset);
    }
    else
    {
        Vector2f viewSize = m_sfView.getSize();
        Vector2f adjustedTargetPosition(RoundFloor(m_targetPosition.x), RoundFloor(m_targetPosition.y));
        Vector2f adjustedHalfSize(RoundFloor(viewSize.x / 2.f), RoundFloor(viewSize.y / 2.f));

        Vector2f offset(
            (int)viewSize.x % 2 == 1 ? 0.5f : 0.f,
            (int)viewSize.y % 2 == 1 ? 0.5f : 0.f
        );

        m_sfView.setCenter(adjustedTargetPosition + offset + adjustedHalfSize);
    }
}

Vector2f Camera::GetPickedPosition(const Vector2i& _kMouseCoords) const
{
    return m_window->GetSFRenderWindow()->mapPixelToCoords(_kMouseCoords, m_sfView);
}

bool Camera::IsMouseOverCamera(const Vector2i& _kMouseCoords) const
{
    sf::IntRect viewport = m_window->GetSFRenderWindow()->getViewport(m_sfView);
    return viewport.contains(_kMouseCoords);
}

bool Camera::IsMouseOverElement(const Vector2i& _kMouseCoords, Element* _pElement) const
{
    Vector2f localPickedCoords;
    return IsMouseOverElement(_kMouseCoords, _pElement, localPickedCoords);
}

bool Camera::IsMouseOverElement(const Vector2i& _kMouseCoords, Element* _pElement, Vector2f& localPickedCoords) const
{
    if (!IsMouseOverCamera(_kMouseCoords))
        return false;

    const Element* pTopParent = _pElement->GetTopParent();
    bool bSceneNode = false;
    bool bWindowNode = false;

    // If this camera is associated to a scene, it should not consider nodes from its associated window.
    if (m_scene)
        bSceneNode = (m_scene->GetRootNode() == pTopParent);
    else if (m_window)
        bWindowNode = (m_window->GetRootNode() == pTopParent);

    if (!bSceneNode && !bWindowNode)
        return false;

    return _pElement->IsPicked(GetPickedPosition(_kMouseCoords), localPickedCoords);
}

}   // namespace gugu
