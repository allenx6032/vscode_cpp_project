#ifndef TRAP_ENCOUNTER_CPP
#define TRAP_ENCOUNTER_CPP

#include <sstream>
#include "designer_trap_encounter.hpp"
#include "designer_trap.hpp"

const int MAX_TRAPS = 2;

TrapEncounter::TrapEncounter() {
    int num_traps = (rand() % MAX_TRAPS) + 1;
    for(int i = 0; i<num_traps; i++) {
        this->traps.push_back(Trap());
    }
}

TrapEncounter::TrapEncounter(std::vector<Trap> traps) : traps(traps) {}

TrapEncounter::~TrapEncounter() {
    // for (auto trap : this->traps) {
    //     delete trap;
    // }
}

std::string TrapEncounter::getEncounter() {
    std::stringstream rtn;
    // Trap Encounter!
    //      Bear Trap causing Slowness.
    //      Trap Door causing Weakness.
    rtn << "Trap Encounter!\n";
    for (int i = 0; i < this->traps.size(); i++) {
        rtn << "\t" << this->traps[i].getName() << " causing " << this->traps[i].getEffects() << ".";
        if (i != this->traps.size() - 1) {
            rtn << std::endl;
        }
    }

    return rtn.str();
}

Encounter* TrapEncounter::clone() {
    return new TrapEncounter(this->traps);
}

#endif // TRAP_ENCOUNTER_CPP