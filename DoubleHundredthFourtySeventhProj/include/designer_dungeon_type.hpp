#ifndef DUNGEON_TYPE_HPP
#define DUNGEON_TYPE_HPP

#include "designer_room.hpp"
#include "designer_matrix_graph.hpp"

class DungeonType
{
private:
public:

	~DungeonType() {};
	
	virtual void generateDungeon(MatrixGraph<Room>& rooms) = 0;
	virtual std::string displayDungeon(MatrixGraph<Room>& rooms) = 0;
};

#endif //DUNGEON_TYPE_HPP