
#ifndef ME_NPC_HPP
#define ME_NPC_HPP

#include <deque>
#include <stack>

#include "FallingSand_core.hpp"
#include "FallingSand_player.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "FallingSand_textures.hpp"

namespace ME {

class State {
public:
    // Basic set of mandate requirements
    MEvec2 pos;
    MEvec2 range = {0, 0};
    std::string task = "Null";

    // Target Bot
    // int owner; //Owner of this Memory
    // int starter; //Originator of this Memory
    // BlockType block = BLOCK_AIR;
    bool reachable = true;
    int time = 0;
    int target = 0;

    // Get the Current Local State
    void retrieveState(world &world, int bot);

    // Overloaded Operators
    void operator=(const State &rhs) {
        // Set all properties
        pos = rhs.pos;
        target = rhs.target;
        task = rhs.task;
        // block = rhs.block;
        reachable = rhs.reachable;
        time = rhs.time;
        range = rhs.range;
    }
};

// Compare States, return a vector of keys?
inline bool operator==(State lhs, const State &rhs) {
    // Check all Properties
    if (!ME_vec2_equals(lhs.pos, rhs.pos)) {
        return false;
    }
    if (lhs.task != rhs.task) {
        return false;
    }
    // if (lhs.block != rhs.block) {
    //     return false;
    // }
    if (lhs.reachable != rhs.reachable) {
        return false;
    }
    if (lhs.time != rhs.time) {
        return false;
    }
    if (lhs.target != rhs.target) {
        return false;
    }
    if (!ME_vec2_equals(lhs.range, rhs.range)) {
        return false;
    }
    return true;
}

// Compare States, return a vector of keys?
inline bool operator||(State lhs, const State &rhs) {
    // Check all Properties
    if (ME_vec2_equals(lhs.pos, rhs.pos)) {
        return true;
    }
    if (lhs.task == rhs.task) {
        return true;
    }
    // if (lhs.block == rhs.block) {
    //     return true;
    // }
    if (lhs.reachable == rhs.reachable) {
        return true;
    }
    if (lhs.time == rhs.time) {
        return true;
    }
    if (lhs.target == rhs.target) {
        return true;
    }
    if (ME_vec2_equals(lhs.range, rhs.range)) {
        return true;
    }
    return false;
}

struct Memory {
    State state;
    int recallScore;
};

struct Task {};

class Bot {
private:
    unsigned int memorySize;
    unsigned int shortermSize;

public:
    Bot(int _ID);

    Bot(const Bot &p) = default;

    Item *heldItem = nullptr;
    f32 holdAngle = 0;
    i64 startThrow = 0;
    EnumPlayerHoldType holdtype = HoldTypeNone;
    int hammerX = 0;
    int hammerY = 0;

    // Meta Data
    MEvec2 pos;
    MEvec2 range = {0, 0};
    MEvec2 viewDistance;

    std::string species;
    bool dead = false;
    int ID;

    // Bot has a sprite
    Texture *sprite;
    void setupSprite();

    void render(WorldEntity *we, R_Target *target, int ofsX, int ofsY);
    void renderLQ(WorldEntity *we, R_Target *target, int ofsX, int ofsY);

    // Bot Activities and Actions
    Task *current;
    bool interrupt = false;           // Interrupt flag
    bool tryInterrupt(State _state);  // Attempt an Interruption of the Bot
    std::string task = "Idle";        // Task Name
    std::stack<MEvec2> path;          // Movement Path
    void executeTask(world &world);

    // Memories / Brain
    std::deque<Memory> shorterm;   // Shortterm Sensory Memory
    std::deque<Memory> memories;   // Longterm Memory
    std::vector<Task *> mandates;  // Mandate Memory

    std::deque<Memory> recallMemories(Memory &query, bool all);
    void updateMemory(Memory &query, bool all, Memory &memory);
    inline void addMemory(State &state);
};

// Constructors
inline Bot::Bot(int _ID) {
    viewDistance = {2, 2};
    memorySize = 25;
    shortermSize = 5;
    species = "Human";
    ID = _ID;
    pos = {0, 0};
}

class NpcSystem : public ecs::system<move_player_event> {
public:
    void process(ecs::registry &world, const move_player_event &evt) override;
};

}  // namespace ME

#endif