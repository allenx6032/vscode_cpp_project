////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_AudioMixerGroupInstance.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ManagerAudio.h"
#include "Gugu_AudioMixerGroup.h"
#include "Gugu_Container.h"

#include <assert.h>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

AudioMixerGroupInstance::AudioMixerGroupInstance(AudioMixerGroup* mixerGroup)
    : m_mixerGroup(mixerGroup)
    , m_parentMixerGroupInstance(nullptr)
    , m_volume(1.f)
    , m_muted(false)
{
}

AudioMixerGroupInstance::~AudioMixerGroupInstance()
{
    ClearStdVector(m_childMixerGroupInstances);
}

void AudioMixerGroupInstance::SetVolume(float volume)
{
    m_volume = volume;

    GetAudio()->RecomputeAllMixedVolumes();
}

float AudioMixerGroupInstance::GetVolume() const
{
    return m_volume;
}

void AudioMixerGroupInstance::SetMuted(bool muted)
{
    m_muted = muted;

    GetAudio()->RecomputeAllMixedVolumes();
}

bool AudioMixerGroupInstance::IsMuted() const
{
    return m_muted;
}

float AudioMixerGroupInstance::ComputeMixedVolume(float volume) const
{
    if (m_muted)
        return 0.f;

    volume = volume * m_mixerGroup->GetVolumeAttenuation() * m_volume;

    if (m_parentMixerGroupInstance)
    {
        volume = m_parentMixerGroupInstance->ComputeMixedVolume(volume);
    }

    return volume;
}

void AudioMixerGroupInstance::LoadMixerGroupHierarchy(AudioMixerGroupInstance* parentMixerGroupInstance, std::map<AudioMixerGroup*, AudioMixerGroupInstance*>& registeredMixerGroupInstances)
{
    m_parentMixerGroupInstance = parentMixerGroupInstance;

    for (const auto& childMixerGroup : m_mixerGroup->GetChildMixerGroups())
    {
        if (childMixerGroup == nullptr)
            continue;

        // Check redundancy and recursive loops.
        if (StdMapContainsKey(registeredMixerGroupInstances, childMixerGroup))
        {
            assert(false);   // An AudioMixerGroup should appear only once in the AudioMixer hierarchy.
            continue;
        }

        AudioMixerGroupInstance* childMixerGroupInstance = new AudioMixerGroupInstance(childMixerGroup);
        registeredMixerGroupInstances.insert(std::make_pair(childMixerGroup, childMixerGroupInstance));
        m_childMixerGroupInstances.push_back(childMixerGroupInstance);

        childMixerGroupInstance->LoadMixerGroupHierarchy(this, registeredMixerGroupInstances);
    }
}

}   // namespace gugu
