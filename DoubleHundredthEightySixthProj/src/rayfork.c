#include "rayfork.h"

#include "rayfork-core.c.h"
#include "rayfork-str.c.h"
#include "rayfork-math.c.h"

#if !defined(RAYFORK_NO_GFX)
#include "rayfork-gfx.c.h"
#endif

#if defined(RAYFORK_NO_AUDIO)
#include "rayfork-audio.c.h"
#endif