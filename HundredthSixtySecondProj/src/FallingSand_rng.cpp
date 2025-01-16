// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#include "FallingSand_rng.h"

#include "FallingSand_mathlib.hpp"
#include "FallingSand_utility.hpp"

RNG* RNG_Create() {
    srand(time(NULL));
    RNG* sRNG = new RNG;
    sRNG->root_seed = rand();
    return sRNG;
}

void RNG_Delete(RNG* rng) {
    if (NULL != rng) delete rng;
}

u32 RNG_Next(RNG* rng) { return rand(); }
