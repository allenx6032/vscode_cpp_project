#ifndef LOOT_ENCOUNTER_FACTORY_TEST_HPP
#define LOOT_ENCOUNTER_FACTORY_TEST_HPP

#include "gtest.h"
#include "gmock.h"
#include "designer_abstract_encounter_factory.hpp"
#include "designer_loot_encounter_factory.hpp"

TEST(LootEncounterFactoryTest, EncounterNotEmpty) {
    AbstractEncounterFactory* factory = new LootEncounterFactory();
    Encounter* enc = factory->genEncounter();

    EXPECT_NE(enc->getEncounter(), "");

    delete enc;
    delete factory;
}

TEST(LootEncounterFactoryTest, GetEncounterHasLoot) {
    AbstractEncounterFactory* factory = new LootEncounterFactory();
    Encounter* enc = factory->genEncounter();

    EXPECT_THAT(enc->getEncounter(), ::testing::HasSubstr("Loot Encounter!"));

    delete enc;
    delete factory;
}

#endif // LOOT_ENCOUNTER_FACTORY_TEST_HPP