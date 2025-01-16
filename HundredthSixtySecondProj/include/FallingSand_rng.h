// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#include "FallingSand_core.hpp"
#include "FallingSand_mathlib.hpp"

// Random number generation
typedef struct RNG {
    unsigned int root_seed;
} RNG;

RNG* RNG_Create();
void RNG_Delete(RNG* rng);
u32 RNG_Next(RNG* rng);