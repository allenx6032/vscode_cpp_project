////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ElementParticles.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ParticleEffect.h"
#include "Gugu_ManagerVisualEffects.h"
#include "Gugu_ParticleSystem.h"
#include "Gugu_Memory.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementParticles::ElementParticles()
    : m_particleSystem(nullptr)
{
}

ElementParticles::~ElementParticles()
{
    GetVisualEffects()->RemoveParticleSystem(m_particleSystem);
    SafeDelete(m_particleSystem);
}

ParticleSystem* ElementParticles::CreateParticleSystem(ParticleEffect* particleEffect, bool startNow)
{
    return CreateParticleSystem(*particleEffect->GetParticleSettings(), startNow);
}

ParticleSystem* ElementParticles::CreateParticleSystem(const ParticleSystemSettings& settings, bool startNow)
{
    if (m_particleSystem)
    {
        GetVisualEffects()->RemoveParticleSystem(m_particleSystem);
        SafeDelete(m_particleSystem);
    }

    m_particleSystem = new ParticleSystem;
    m_particleSystem->Init(settings);
    m_particleSystem->AttachToElement(this);
    GetVisualEffects()->AddParticleSystem(m_particleSystem);

    if (startNow)
    {
        m_particleSystem->Start();
    }

    return m_particleSystem;
}

ParticleSystem* ElementParticles::GetParticleSystem() const
{
    return m_particleSystem;
}

void ElementParticles::StartParticleSystem()
{
    m_particleSystem->Start();
}

void ElementParticles::RestartParticleSystem()
{
    m_particleSystem->Restart();
}

void ElementParticles::StopParticleSystem()
{
    m_particleSystem->Stop();
}

void ElementParticles::StopEmitting()
{
    m_particleSystem->StopEmitting();
}

void ElementParticles::RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf)
{
    m_particleSystem->Render(_kRenderPass, _kTransformSelf);
}

}   // namespace gugu
