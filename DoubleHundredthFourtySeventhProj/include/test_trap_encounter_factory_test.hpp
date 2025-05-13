#ifndef TRAP_ENCOUNTER_FACTORY_TEST_HPP
#define TRAP_ENCOUNTER_FACTORY_TEST_HPP

#include "gtest.h"
#include "gmock.h"
#include "designer_abstract_encounter_factory.hpp"
#include "designer_trap_encounter_factory.hpp"

TEST(TrapEncounterFactoryTest, EncounterNotEmpty) {
    AbstractEncounterFactory* factory = new TrapEncounterFactory();
    Encounter* enc = factory->genEncounter();

    EXPECT_NE(enc->getEncounter(), "");

    delete enc;
    delete factory;
}

TEST(TrapEncounterFactoryTest, GetEncounterHasTrap) {
    AbstractEncounterFactory* factory = new TrapEncounterFactory();
    Encounter* enc = factory->genEncounter();

    EXPECT_THAT(enc->getEncounter(), ::testing::HasSubstr("Trap Encounter!"));

    delete enc;
    delete factory;
}

#endif // TRAP_ENCOUNTER_FACTORY_TEST_HPP