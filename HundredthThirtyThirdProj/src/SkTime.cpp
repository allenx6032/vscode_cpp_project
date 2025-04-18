/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkString.h"
#include "SkTime.h"

#ifdef SK_BUILD_FOR_WIN

#ifdef SK_DEBUG
SkMSec gForceTickCount = (SkMSec) -1;
#endif

void SkTime::GetDateTime(DateTime* t) {
    if (t) {
        SYSTEMTIME  syst;

        ::GetLocalTime(&syst);
        t->fYear        = SkToU16(syst.wYear);
        t->fMonth       = SkToU8(syst.wMonth);
        t->fDayOfWeek   = SkToU8(syst.wDayOfWeek);
        t->fDay         = SkToU8(syst.wDay);
        t->fHour        = SkToU8(syst.wHour);
        t->fMinute      = SkToU8(syst.wMinute);
        t->fSecond      = SkToU8(syst.wSecond);
    }
}

SkMSec SkTime::GetMSecs() {
#ifdef SK_DEBUG
    if (gForceTickCount != (SkMSec) -1) {
        return gForceTickCount;
    }
#endif
    return ::GetTickCount();
}

#elif defined(xSK_BUILD_FOR_MAC)

#include <time.h>

void SkTime::GetDateTime(DateTime* t) {
    if (t) {
        tm      syst;
        time_t  tm;

        time(&tm);
        localtime_r(&tm, &syst);
        t->fYear        = SkToU16(syst.tm_year);
        t->fMonth       = SkToU8(syst.tm_mon + 1);
        t->fDayOfWeek   = SkToU8(syst.tm_wday);
        t->fDay         = SkToU8(syst.tm_mday);
        t->fHour        = SkToU8(syst.tm_hour);
        t->fMinute      = SkToU8(syst.tm_min);
        t->fSecond      = SkToU8(syst.tm_sec);
    }
}

SkMSec SkTime::GetMSecs() {
    UnsignedWide    wide;
    ::Microseconds(&wide);

    int64_t s = ((int64_t)wide.hi << 32) | wide.lo;
    s = (s + 500) / 1000;   // rounded divide
    return (SkMSec)s;
}

#endif


void SkTime::DateTime::toISO8601(SkString* dst) const {
    if (dst) {
        int timeZoneMinutes = SkToInt(fTimeZoneMinutes);
        char timezoneSign = timeZoneMinutes >= 0 ? '+' : '-';
        int timeZoneHours = abs(timeZoneMinutes) / 60;
        timeZoneMinutes = abs(timeZoneMinutes) % 60;
        dst->printf("%04u-%02u-%02uT%02u:%02u:%02u%c%02d:%02d",
                    static_cast<unsigned>(fYear), static_cast<unsigned>(fMonth),
                    static_cast<unsigned>(fDay), static_cast<unsigned>(fHour),
                    static_cast<unsigned>(fMinute),
                    static_cast<unsigned>(fSecond), timezoneSign, timeZoneHours,
                    timeZoneMinutes);
    }
}

