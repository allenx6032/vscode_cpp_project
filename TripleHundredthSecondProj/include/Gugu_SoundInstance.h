#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include <SFML_Sound.hpp>

#include <string>

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class AudioClip;
    class AudioMixerGroupInstance;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

struct SoundParameters
{
    AudioClip* audioClip;
    std::string audioClipId;
    AudioMixerGroupInstance* mixerGroupInstance;
    std::string mixerGroupId;
    float volume;
    float pitchLowerOffset;
    float pitchUpperOffset;

    SoundParameters();
};

class SoundInstance
{
public:

    SoundInstance();
    ~SoundInstance();

    void Reset();

    void SetAudioClip(AudioClip* audioClip);
    void SetMixerGroupInstance(AudioMixerGroupInstance* mixerGroupInstance);
    void SetVolume(float volume);
    void SetPitch(float pitch);

    void RecomputeMixedVolume();

    void Play();

protected:

    AudioMixerGroupInstance* m_mixerGroupInstance;
    sf::Sound m_sfSound;
    float m_volume;
};

}   // namespace gugu
