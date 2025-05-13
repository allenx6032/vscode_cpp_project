#ifndef TRAP_ENCOUNTER_FACTORY_CPP
#define TRAP_ENCOUNTER_FACTORY_CPP

#include "designer_trap_encounter_factory.hpp"
#include "designer_encounter.hpp"
#include "designer_trap_encounter.hpp"

TrapEncounterFactory::TrapEncounterFactory() {}

TrapEncounterFactory::~TrapEncounterFactory() {}

Encounter* TrapEncounterFactory::genEncounter() {
    return new TrapEncounter();
}

#endif // TRAP_ENCOUNTER_FACTORY_CPP