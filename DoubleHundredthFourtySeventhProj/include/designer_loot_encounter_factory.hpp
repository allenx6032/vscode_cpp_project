#ifndef LOOT_ENCOUNTER_FACTORY_HPP
#define LOOT_ENCOUNTER_FACTORY_HPP

#include "designer_abstract_encounter_factory.hpp"
#include "designer_encounter.hpp"

class LootEncounterFactory : public AbstractEncounterFactory
{
private:
    
public:
    LootEncounterFactory();
    ~LootEncounterFactory();
    Encounter* genEncounter();
};

#endif // LOOT_ENCOUNTER_FACTORY_HPP