#ifndef _AL_ERROR_H_
#define _AL_ERROR_H_

#include "al_alMain.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ALboolean TrapALError;

ALvoid alSetError(ALCcontext *Context, ALenum errorCode);

#ifdef __cplusplus
}
#endif

#endif
