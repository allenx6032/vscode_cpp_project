#ifndef LOOT_ENCOUNTER_TEST_HPP
#define LOOT_ENCOUNTER_TEST_HPP

#include "gmock.h"
#include "iostream"
#include <vector>
#include <utility>

#include "designer_loot.hpp"
#include "designer_loot_encounter.hpp"

TEST(LootEncounterTest, EncounterNotEmpty) {
    LootEncounter* loot = new LootEncounter();
	EXPECT_NE(loot->getEncounter(), "");
    delete loot;
}

TEST(LootEncounterTest, GetEncounterHasLoot) {
    LootEncounter* loot = new LootEncounter();
    EXPECT_THAT(loot->getEncounter(), ::testing::HasSubstr("Loot Encounter"));
    delete loot;
}

TEST(LootEncounterTest, SingleLoot) {
    std::vector<std::pair<Loot, int>> items;
    items.push_back(std::pair<Loot, int>(Loot("Computer Monitor"), 20));

    LootEncounter* loot = new LootEncounter(items);

    EXPECT_EQ(loot->getEncounter(), "Loot Encounter!\n\t20 Computer Monitors.");

    delete loot;
}

TEST(LootEncounterTest, ThreeLoot) {
    std::vector<std::pair<Loot, int>> items;
    items.push_back(std::pair<Loot, int>(Loot("Computer Monitor"), 20));
    items.push_back(std::pair<Loot, int>(Loot("Throwing Star"), 4));
    items.push_back(std::pair<Loot, int>(Loot("NVIDIA GTX 3090"), 1));

    LootEncounter* loot = new LootEncounter(items);

    EXPECT_EQ(loot->getEncounter(), "Loot Encounter!\n\t20 Computer Monitors.\n\t4 Throwing Stars.\n\t1 NVIDIA GTX 3090s.");

    delete loot;
}

#endif // LOOT_ENCOUNTER_TEST_HPP