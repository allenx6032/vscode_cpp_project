#include "gameplay-Base.h"
#include "gameplay-Layout.h"
#include "gameplay-Control.h"
#include "gameplay-Container.h"
#include "gameplay-Game.h"

namespace gameplay
{

bool Layout::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    return false;
}

}