#pragma once

#include <SFML_Graphics.hpp>

#include "deceptus_audioupdatedata.h"
#include "deceptus_constants.h"

#include <b2_box2d.h>

#include <memory>
#include <optional>

class Weapon
{
public:
   struct WeaponUpdateData
   {
      const sf::Time& _time;
      std::shared_ptr<b2World> _world;
   };

   Weapon() = default;
   virtual ~Weapon() = default;
   WeaponType getWeaponType() const;

   virtual void draw(sf::RenderTarget& target);
   virtual void update(const WeaponUpdateData& data);
   virtual void initialize();
   virtual int32_t getDamage() const;
   virtual std::string getName() const = 0;

   void setParentAudioUpdateData(const AudioUpdateData& parent_audio_update_data);

protected:
   WeaponType _type = WeaponType::None;
   std::optional<AudioUpdateData> _parent_audio_update_data;
};
