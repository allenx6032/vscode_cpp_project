#ifndef ALTHUNK_H
#define ALTHUNK_H

#include "al_alMain.h"

#ifdef __cplusplus
extern "C" {
#endif

void ThunkInit(void);
void ThunkExit(void);
ALenum NewThunkEntry(ALuint *index);
void FreeThunkEntry(ALuint index);

#ifdef __cplusplus
}
#endif

#endif //ALTHUNK_H

