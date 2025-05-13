#pragma once

//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include <vector>
#include "DungeonRoom.h"
#include "DelaunayTriangulation.h"

//-----------------------------------------------------
// DungeonGenerator Class									
//-----------------------------------------------------
class DungeonGenerator final
{
public:
	DungeonGenerator();				// Constructor
	~DungeonGenerator() = default;	// Destructor

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void GenerateDungeon(std::vector<DungeonRoom>& rooms);
	void Update(std::vector<DungeonRoom>& rooms);

	void SetSeed(int seed) { m_CurrentSeed = seed; }
	void SetInitialRadius(int initRadius) { m_InitRadius = initRadius; }
	void SetInitialRoomCount(int initRoomCount) { m_InitRoomCount = initRoomCount; }
	void SetRoomSizeBounds(int minSize, int maxSize);
	void SetGenerationState(bool isSlowlyGenerating) { m_IsSlowlyGenerating = isSlowlyGenerating; }
	void SetRoomSizeThreshold(int size) { m_RoomSizeThreshold = size; }

	void DrawDebug() const;
	bool IsDone() const;
	int GetInitialRoomCount() const;
	int GetInitialRadius() const;
	
private:
	//-------------------------------------------------
	// Private member functions								
	//-------------------------------------------------
	void CreateRoomsInCircle(std::vector<DungeonRoom>& rooms) const;
	void CreateRoomInCircle(std::vector<DungeonRoom>& rooms) const;
	bool SeperateRooms(std::vector<DungeonRoom>& rooms) const;
	bool DiscardSmallRooms(std::vector<DungeonRoom>& rooms, bool debug = false);
	bool DiscardBorderingRooms(std::vector<DungeonRoom>& rooms, bool debug = false);
	void CreateMinimumSpanningTree();
	void CreateCorridors(std::vector<DungeonRoom>& rooms) const;
	void ChooseBeginAndEndRoom(std::vector<DungeonRoom>& rooms) const;

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	enum class GenerationCycleState
	{
		CIRCLE,
		SEPERATION,
		DISCARD_SMALL_ROOMS,
		DISCARD_BORDERING_ROOMS,
		TRIANGULATION,
		SPANNING_TREE_ALGORITHM,
		CORRIDORS,
		DONE
	};

	bool m_IsGenerating{};
	int m_CurrentSeed{ -1 };

	const Vector2 m_Center{ 300, 300 };
	int m_InitRadius{ 75 };
	int m_InitRoomCount{ 150 };
	Vector2 m_RoomSizeBounds{ 4, 40 };
	int m_RoomSizeThreshold{ 30 };
	int m_CurTriangulateRoom{};

	std::vector<DungeonRoom> m_DebugRooms{};
	std::vector<Edge> m_MinimumSpanningTree{};

	DelaunayTriangulation m_Triangulation{};
	GenerationCycleState m_CurrentGenerationState{};
	bool m_IsSlowlyGenerating{};
};
