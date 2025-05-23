#ifndef _AL_ERROR_H_
#define _AL_ERROR_H_

#include "AL_al.h"
#include "AL_alc.h"

#ifdef __cplusplus
extern "C" {
#endif

ALvoid alSetError(ALCcontext *Context, ALenum errorCode);

#ifdef __cplusplus
}
#endif

#endif
