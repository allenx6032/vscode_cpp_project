#include "gameplay-Base.h"
#include "gameplay-Control.h"
#include "gameplay-AbsoluteLayout.h"
#include "gameplay-Container.h"

namespace gameplay
{

static AbsoluteLayout* __instance;

AbsoluteLayout::AbsoluteLayout()
{
}

AbsoluteLayout::~AbsoluteLayout()
{
    __instance = NULL;
}

AbsoluteLayout* AbsoluteLayout::create()
{
    if (!__instance)
    {
        __instance = new AbsoluteLayout();
    }
    else
    {
        __instance->addRef();
    }

    return __instance;
}

Layout::Type AbsoluteLayout::getType()
{
    return Layout::LAYOUT_ABSOLUTE;
}

void AbsoluteLayout::update(const Container* container)
{
    // Nothing to do for absolute layout
}

}