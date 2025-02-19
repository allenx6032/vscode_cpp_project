
#include "al_config.h"

#include "al_alc_device.h"

#include <cstddef>
#include <numeric>

#include "al_al_buffer.h"
#include "al_al_effect.h"
#include "al_al_filter.h"
#include "al_common_albit.h"
#include "al_alc_alconfig.h"
#include "al_alc_backends_base.h"
#include "al_core_bformatdec.h"
#include "al_core_bs2b.h"
#include "al_core_front_stablizer.h"
#include "al_core_hrtf.h"
#include "al_core_logging.h"
#include "al_core_mastering.h"
#include "al_core_uhjfilter.h"


namespace {

using voidp = void*;

} // namespace


ALCdevice::ALCdevice(DeviceType type) : DeviceBase{type}
{ }

ALCdevice::~ALCdevice()
{
    TRACE("Freeing device %p\n", voidp{this});

    Backend = nullptr;

    size_t count{std::accumulate(BufferList.cbegin(), BufferList.cend(), 0_uz,
        [](size_t cur, const BufferSubList &sublist) noexcept -> size_t
        { return cur + static_cast<uint>(al::popcount(~sublist.FreeMask)); })};
    if(count > 0)
        WARN("%zu Buffer%s not deleted\n", count, (count==1)?"":"s");

    count = std::accumulate(EffectList.cbegin(), EffectList.cend(), 0_uz,
        [](size_t cur, const EffectSubList &sublist) noexcept -> size_t
        { return cur + static_cast<uint>(al::popcount(~sublist.FreeMask)); });
    if(count > 0)
        WARN("%zu Effect%s not deleted\n", count, (count==1)?"":"s");

    count = std::accumulate(FilterList.cbegin(), FilterList.cend(), 0_uz,
        [](size_t cur, const FilterSubList &sublist) noexcept -> size_t
        { return cur + static_cast<uint>(al::popcount(~sublist.FreeMask)); });
    if(count > 0)
        WARN("%zu Filter%s not deleted\n", count, (count==1)?"":"s");
}

void ALCdevice::enumerateHrtfs()
{
    mHrtfList = EnumerateHrtf(configValue<std::string>({}, "hrtf-paths"));
    if(auto defhrtfopt = configValue<std::string>({}, "default-hrtf"))
    {
        auto iter = std::find(mHrtfList.begin(), mHrtfList.end(), *defhrtfopt);
        if(iter == mHrtfList.end())
            WARN("Failed to find default HRTF \"%s\"\n", defhrtfopt->c_str());
        else if(iter != mHrtfList.begin())
            std::rotate(mHrtfList.begin(), iter, iter+1);
    }
}

auto ALCdevice::getOutputMode1() const noexcept -> OutputMode1
{
    if(mContexts.load(std::memory_order_relaxed)->empty())
        return OutputMode1::Any;

    switch(FmtChans)
    {
    case DevFmtMono: return OutputMode1::Mono;
    case DevFmtStereo:
        if(mHrtf)
            return OutputMode1::Hrtf;
        else if(mUhjEncoder)
            return OutputMode1::Uhj2;
        return OutputMode1::StereoBasic;
    case DevFmtQuad: return OutputMode1::Quad;
    case DevFmtX51: return OutputMode1::X51;
    case DevFmtX61: return OutputMode1::X61;
    case DevFmtX71: return OutputMode1::X71;
    case DevFmtX714:
    case DevFmtX3D71:
    case DevFmtAmbi3D:
        break;
    }
    return OutputMode1::Any;
}
