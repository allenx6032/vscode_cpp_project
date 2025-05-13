#include "gtest.h"
#include "gmock.h"

#include "test_gridded_dungeon_test.hpp"
#include "test_linear_dungeon_test.hpp"
#include "test_branching_dungeon_test.hpp"
#include "test_room_test.hpp"
#include "test_graph_test.hpp"
#include "test_loot_test.hpp"
#include "test_enemy_test.hpp"
#include "test_trap_test.hpp"
#include "test_loot_encounter_test.hpp"
#include "test_dungeon_test.hpp"
#include "test_enemy_encounter_test.hpp"
#include "test_trap_encounter_test.hpp"
#include "test_trap_encounter_factory_test.hpp"
#include "test_loot_encounter_factory_test.hpp"
#include "test_enemy_encounter_factory_test.hpp"
#include <time.h>

int main(int argc, char **argv) {
  srand(time(NULL));
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
