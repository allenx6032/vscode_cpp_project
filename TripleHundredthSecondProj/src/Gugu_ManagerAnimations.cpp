////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ManagerAnimations.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Engine.h"
#include "Gugu_ManagerResources.h"
#include "Gugu_SpriteAnimation.h"
#include "Gugu_ElementEventHandler.h"
#include "Gugu_ElementSprite.h"
#include "Gugu_Container.h"
#include "Gugu_Trace.h"
#include "Gugu_EngineStats.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ManagerAnimations::ManagerAnimations()
{
}

ManagerAnimations::~ManagerAnimations()
{
}

void ManagerAnimations::Init(const EngineConfig& config)
{
}

void ManagerAnimations::Release()
{
    DeleteAllAnimations();
}

void ManagerAnimations::Step(const DeltaTime& dt)
{
    //GUGU_SCOPE_TRACE_MAIN("Animations");

    //// TODO: handle step vs update animations.
    //for (SpriteAnimation* animation : m_spriteAnimations)
    //{
    //    animation->StepAnimation(dt);
    //}
}

void ManagerAnimations::Update(const DeltaTime& dt, EngineStats& stats)
{
    GUGU_SCOPE_TRACE_MAIN("Animations");

    // TODO: handle step vs update animations.
    for (SpriteAnimation* animation : m_spriteAnimations)
    {
        animation->StepAnimation(dt);
    }

    stats.animationCount = (int)m_spriteAnimations.size();
}

SpriteAnimation* ManagerAnimations::AddAnimation(ElementSprite* sprite)
{
    if (sprite == nullptr)
        return nullptr;

    SpriteAnimation* animation = new SpriteAnimation;
    animation->SetSprite(sprite);

    sprite->GetEvents()->AddCallback(EElementEvent::Destroyed, Handle(this), std::bind(
        &ManagerAnimations::DeleteAnimationFromSprite,
        this,
        sprite));

    m_spriteAnimations.push_back(animation);
    return animation;
}

void ManagerAnimations::RemoveAnimation(SpriteAnimation* animation)
{
    // RemoveAnimation will be called in the animation destructor.
    StdVectorRemove(m_spriteAnimations, animation);
}

void ManagerAnimations::DeleteAnimation(SpriteAnimation* animation)
{
    // RemoveAnimation will be called in the animation destructor.
    SafeDelete(animation);
}

void ManagerAnimations::DeleteAnimationFromSprite(ElementSprite* sprite)
{
    if (sprite == nullptr)
        return;

    size_t i = 0;
    while (i < m_spriteAnimations.size())
    {
        if (m_spriteAnimations[i]->GetSprite() == sprite)
        {
            SpriteAnimation* animation = m_spriteAnimations[i];
            StdVectorRemoveAt(m_spriteAnimations, i);

            DeleteAnimation(animation);
        }
        else
        {
            ++i;
        }
    }
}

void ManagerAnimations::DeleteAllAnimations()
{
    //TODO: I should use some flag to tell if an animation is being destroyed, and avoid going back and forth between the manager and the destructors.
    for (size_t i = 0; i < m_spriteAnimations.size(); ++i)
    {
        SpriteAnimation* animation = m_spriteAnimations[i];
        m_spriteAnimations[i] = nullptr;  //Set to null before calling the delete, to avoid the child to call a remove

        SafeDelete(animation);
    }

    m_spriteAnimations.clear();
}

ManagerAnimations* GetAnimations()
{
    return GetEngine()->GetManagerAnimations();
}

}   // namespace gugu
