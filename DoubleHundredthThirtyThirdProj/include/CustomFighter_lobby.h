#ifndef NET_LOBBY_H
#define NET_LOBBY_H

#include "CustomFighter_game_state.h"
#include "CustomFighter_player.h"

namespace Lobby {

	struct PlayerData {
		std::string remote;
		Player::Config config;
	};

	struct Room {
		int code = -1;
		std::string password = "";

		int player_max = 2;
		int round_max = 3;
		int timer_max = 60;
		int game_mode = GameMode::Versus;

		int player_count = 0;
		PlayerData player_data[MAX_PLAYERS];

		bool good();
	};

	Room run(Player::Config config);
};

#endif