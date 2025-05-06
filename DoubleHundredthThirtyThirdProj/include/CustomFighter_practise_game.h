#ifndef PRACTISE_GAME_H
#define PRACTISE_GAME_H

#include "CustomFighter_game_state.h"
#include "CustomFighter_player.h"

namespace PractiseGame {
	bool run(std::vector<Player::Config> configs);
};

#endif