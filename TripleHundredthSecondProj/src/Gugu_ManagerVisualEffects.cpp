////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ManagerVisualEffects.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Engine.h"
#include "Gugu_ManagerResources.h"
#include "Gugu_ParticleSystem.h"
#include "Gugu_SpriteAnimation.h"
#include "Gugu_ElementSprite.h"
#include "Gugu_Container.h"
#include "Gugu_Trace.h"
#include "Gugu_EngineStats.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ManagerVisualEffects::ManagerVisualEffects()
{
}

ManagerVisualEffects::~ManagerVisualEffects()
{
}

void ManagerVisualEffects::Init(const EngineConfig& config)
{
}

void ManagerVisualEffects::Release()
{
    DeleteAllParticleSystems();
}

void ManagerVisualEffects::Update(const DeltaTime& dt, EngineStats& stats)
{
    GUGU_SCOPE_TRACE_MAIN("Visual Effects");

    for (ParticleSystem* particleSystem : m_particleSystems)
    {
        GUGU_SCOPE_TRACE_MAIN_("Particles", Particles);

        particleSystem->Update(dt);
    }

    stats.particleSystemCount = (int)m_particleSystems.size();
}

void ManagerVisualEffects::AddParticleSystem(ParticleSystem* particleSystem)
{
    if (!particleSystem)
        return;

    m_particleSystems.push_back(particleSystem);
}

void ManagerVisualEffects::RemoveParticleSystem(ParticleSystem* particleSystem)
{
    StdVectorRemove(m_particleSystems, particleSystem);
}

void ManagerVisualEffects::DeleteAllParticleSystems()
{
    for (size_t i = 0; i < m_particleSystems.size(); ++i)
    {
        ParticleSystem* particleSystem = m_particleSystems[i];
        m_particleSystems[i] = nullptr;

        SafeDelete(particleSystem);
    }

    m_particleSystems.clear();
}

ManagerVisualEffects* GetVisualEffects()
{
    return GetEngine()->GetManagerVisualEffects();
}

}   // namespace gugu
