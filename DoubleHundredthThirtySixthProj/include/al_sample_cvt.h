#ifndef SAMPLE_CVT_H
#define SAMPLE_CVT_H

#include "al_al.h"
#include "al_alBuffer.h"

void ConvertData(ALvoid *dst, enum UserFmtType dstType, const ALvoid *src, enum UserFmtType srcType, ALsizei numchans, ALsizei len, ALsizei align);

#endif /* SAMPLE_CVT_H */
