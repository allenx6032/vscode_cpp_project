
#include "al_config.h"

#include "al_core_bformatdec.h"
#include "al_core_bs2b.h"
#include "al_core_device.h"
#include "al_core_front_stablizer.h"
#include "al_core_hrtf.h"
#include "al_core_mastering.h"


static_assert(std::atomic<std::chrono::nanoseconds>::is_always_lock_free);


DeviceBase::DeviceBase(DeviceType type)
    : Type{type}, mContexts{al::FlexArray<ContextBase*>::Create(0)}
{
}

DeviceBase::~DeviceBase() = default;
