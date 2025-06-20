#pragma once

#include "deceptus_animation.h"
#include "deceptus_animationsettings.h"
#include "deceptus_constants.h"

#include <array>
#include <list>
#include <memory>

class AnimationPool
{
public:
   AnimationPool(const std::string& file_path);

   void initialize();
   void reload();

   std::shared_ptr<Animation>
   create(const std::string& name, float x = 0.0f, float y = 0.0f, bool auto_play = true, bool managed_by_pool = true);

   void drawAnimations(sf::RenderTarget& target, const std::vector<std::string>& animations = {});
   void drawAnimations(sf::RenderTarget& target, sf::RenderTarget& normal, const std::vector<std::string>& animations = {});

   void updateAnimations(const sf::Time& dt);
   const std::map<std::string, std::shared_ptr<Animation>>& getAnimations();

   void setGarbageCollectorEnabled(bool newGarbage_collector_enabled);

   const std::map<std::string, std::shared_ptr<AnimationSettings>>& settings() const;

   void saveToJson();
   void reloadFromJson();

   enum class UpdateFlag
   {
      Settings,
      Texture,
      NormalMap,
      All
   };

   void recreateAnimationsFromSettings(UpdateFlag flag = UpdateFlag::Settings);

private:
   void deserialize(const std::string& data);
   void deserializeFromFile(const std::string& filename);

   bool _initialized = false;

   std::map<std::string, std::shared_ptr<AnimationSettings>> _settings;
   std::map<std::string, std::shared_ptr<Animation>> _animations;
   std::string _file_path;
   bool _garbage_collector_enabled{true};
};
