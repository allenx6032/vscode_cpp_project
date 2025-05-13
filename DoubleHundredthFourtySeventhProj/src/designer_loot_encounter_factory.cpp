#ifndef LOOT_ENCOUNTER_FACTORY_CPP
#define LOOT_ENCOUNTER_FACTORY_CPP

#include "designer_loot_encounter_factory.hpp"
#include "designer_encounter.hpp"
#include "designer_loot_encounter.hpp"

LootEncounterFactory::LootEncounterFactory() {}

LootEncounterFactory::~LootEncounterFactory() {}

Encounter* LootEncounterFactory::genEncounter() {
    return new LootEncounter();
}

#endif // LOOT_ENCOUNTER_FACTORY_CPP