#ifndef DUNGEON_MANAGER_HPP
#define DUNGEON_MANAGER_HPP

//#include <iostream>
#include <string>
#include <limits>
#include "designer_dungeon.hpp"

class DungeonManager {
	private:

	public:
		DungeonManager();

		void displayMainMenu();
		void displayGenerationMenu();
		void displayDungeonDisplayMenu();
		void generateDungeon();
		void clearScreen();
};

#endif //DUNGEON_MANAGER_HPP
