#pragma once

#include "deceptus_audiorange.h"
#include "deceptus_audioupdatedata.h"
#include "deceptus_constants.h"
#include "deceptus_chunk.h"

#include "SFML_Graphics.hpp"

#include "json.hpp"

#include <cstdint>
#include <optional>

struct Room;

class GameMechanism
{
public:
   GameMechanism() = default;
   virtual ~GameMechanism() = default;

   virtual void draw(sf::RenderTarget& target, sf::RenderTarget& normal);
   virtual void update(const sf::Time& dt);

   virtual bool isEnabled() const;
   virtual void setEnabled(bool enabled);
   virtual void toggle();

   virtual bool isVisible() const;
   virtual void setVisible(bool visible);

   virtual void preload();

   // audio related
   virtual bool hasAudio() const;
   virtual std::optional<AudioRange> getAudioRange() const;
   virtual bool isAudioEnabled() const;
   virtual void setAudioEnabled(bool audio_enabled);
   virtual void setReferenceVolume(float volume);  // this is read only for the volume updater
   virtual void setVolume(float volume);
   virtual float getReferenceVolume() const;
   virtual AudioUpdateBehavior getAudioUpdateBehavior() const;
   virtual void setAudioUpdateBehavior(AudioUpdateBehavior audio_update_behavior);
   virtual const std::vector<int32_t>& getRoomIds() const;
   virtual void setRoomIds(const std::vector<int32_t>& room_ids);
   virtual void addRoomId(int32_t room_id);

   // use when mechanism update/draw calls are expensive
   virtual bool hasChunks() const;
   virtual const std::vector<Chunk>& getChunks() const;
   virtual void addChunks(const sf::FloatRect& bounding_box);

   virtual int32_t getZ() const;
   virtual void setZ(const int32_t& z);

   virtual std::optional<sf::FloatRect> getBoundingBoxPx() = 0;

   virtual void serializeState(nlohmann::json&);
   virtual void deserializeState(const nlohmann::json&);
   virtual bool isSerialized() const;

protected:
   int32_t _z_index{0};
   bool _enabled{true};
   bool _visible{true};
   bool _serialized{false};
   bool _observed{false};

   // audio related
   bool _has_audio{false};
   bool _audio_enabled{false};
   float _reference_volume{0.0f};
   AudioUpdateData _audio_update_data;

   std::vector<Chunk> _chunks;
   MechanismVersion _version = MechanismVersion::Version1;
};
