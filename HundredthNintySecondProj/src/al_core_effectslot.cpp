
#include "al_config.h"

#include "al_core_effectslot.h"

#include <cstddef>

#include "al_common_almalloc.h"
#include "al_core_context.h"


std::unique_ptr<EffectSlotArray> EffectSlot::CreatePtrArray(size_t count)
{
    /* Allocate space for twice as many pointers, so the mixer has scratch
     * space to store a sorted list during mixing.
     */
    return std::unique_ptr<EffectSlotArray>{new(FamCount{count*2}) EffectSlotArray(count)};
}
