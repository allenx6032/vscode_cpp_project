#include "deceptus_playerutils.h"

#include "deceptus_player.h"

sf::Vector2f PlayerUtils::getPixelPositionFloat()
{
   return Player::getCurrent()->getPixelPositionFloat();
}
