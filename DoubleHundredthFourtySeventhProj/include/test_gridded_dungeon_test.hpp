#ifndef GRIDDED_DUNGEON_TEST_HPP
#define GRIDDED_DUNGEON_TEST_HPP

#include "gtest.h"
#include "gmock.h"

//#include "designer_matrix_graph.hpp"
#include "designer_gridded_dungeon_type.hpp"

TEST(GriddedDungeonTest, Generate1RoomGriddedDungeonTest) {
    GriddedDungeonType* type = new GriddedDungeonType();
    MatrixGraph<Room> graph(1);
    Room room(1);
    graph.addVertex(room);
    type->generateDungeon(graph);
    
    string test = "Here is the adjacency matrix:\n"
        " id  0  \n"
        "  0  0  \n";

    EXPECT_EQ(type->displayDungeon(graph), test);
    EXPECT_EQ(graph.getSize(), 1);

    delete type;
}

TEST(GriddedDungeonTest, Generate0RoomGriddedDungeonTest) {
    GriddedDungeonType* type = new GriddedDungeonType();
    MatrixGraph<Room> graph(0);
    type->generateDungeon(graph);

    string test = "Here is the adjacency matrix:\n"
        " id \n";

    EXPECT_EQ(type->displayDungeon(graph), test);
    EXPECT_EQ(graph.getSize(), 0);

    delete type;
}

TEST(GriddedDungeonTest, Generate2RoomGriddedDungeonTest) {
    GriddedDungeonType* type = new GriddedDungeonType();
    MatrixGraph<Room> graph(2);
    Room room1(1);
    Room room2(2);
    graph.addVertex(room1);
    graph.addVertex(room2);

    type->generateDungeon(graph);

    string test = "Here is the adjacency matrix:\n"
        " id  0   1  \n"
        "  0  0   1  \n"
        "  1  1   0  \n";

    EXPECT_EQ(type->displayDungeon(graph), test);
    EXPECT_EQ(graph.getSize(), 2);

    delete type;
}

TEST(GriddedDungeonTest, Generate10RoomGriddedDungeonTest) {
    GriddedDungeonType* type = new GriddedDungeonType();
    MatrixGraph<Room> graph(10);
    for (int i = 0; i < graph.getMaxSize(); i++)
    {
        Room room(i);
        graph.addVertex(room);
    }

    type->generateDungeon(graph);
    string dungeon1 = type->displayDungeon(graph);
    type->generateDungeon(graph);
    string dungeon2 = type->displayDungeon(graph);

    // NOTE: technically, dungeon1 and dungeon2 have a chance to be the same,
    // but it's such a small chance for large dungeons that we assume it will never happen
    EXPECT_NE(dungeon1, dungeon2);
    EXPECT_EQ(graph.getSize(), 10);

    delete type;
}

#endif // GRIDDED_DUNGEON_TEST_HPP