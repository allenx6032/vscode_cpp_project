#ifndef VOLUMEUPDATER_H
#define VOLUMEUPDATER_H

#include <SFML_Graphics.hpp>

#include "deceptus_luanode.h"
#include "deceptus_gamemechanism.h"

#include <set>
#include <thread>

class Projectile;

class VolumeUpdater
{
public:
   VolumeUpdater() = default;

   void update();
   void updateProjectiles(const std::set<Projectile*>& projectiles);

   void setPlayerPosition(const sf::Vector2f& position);
   void setMechanisms(const std::vector<std::vector<std::shared_ptr<GameMechanism>>*>& mechanisms);
   void setRoomId(const std::optional<int32_t>& room_id);

private:
   void updateVolume(const std::shared_ptr<GameMechanism>& mechanism);
   float computeDistanceToPlayerPx(const std::shared_ptr<GameMechanism>& mechanism);

   std::vector<std::vector<std::shared_ptr<GameMechanism>>*> _mechanisms;
   sf::Vector2f _player_position;

   std::unique_ptr<std::thread> _thread;
   std::atomic<bool> _stopped{false};
   std::optional<int32_t> _room_id;
};

#endif  // VOLUMEUPDATER_H
