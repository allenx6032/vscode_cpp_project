// Copyright (C) 2011-2023 Valeriano Alfonso Rodriguez (Kableado)

#include <sys/time.h>
#include <unistd.h>

#include "GameLib_TimeUtils.h"

/////////////////////////////
// Time_GetTime
//
// Gets the current time in usecs.
/////////////////////////////
// Time_Pause
//
// Pauses the execution for t usecs.
#if WIN32
#include <windows.h>
// WIN32
long long Time_GetTime() {
	LARGE_INTEGER freq;
	LARGE_INTEGER tim;
	long long int microt;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&tim);
	microt = (tim.QuadPart * 1000000) / freq.QuadPart;
	return (microt);
}
void Time_Pause(long long pausa) {
	long long tend, t, diff;

	t    = Time_GetTime();
	tend = t + pausa;
	do {
		diff = tend - t;
		if (diff > 1000) {
			Sleep(diff / 1000);
		} else {
			Sleep(0);
		}
		t = Time_GetTime();
	} while (tend >= t);
}
#else
// UNIX
long long Time_GetTime() {
	struct timeval t;
	long long usecs;
	gettimeofday(&t, NULL);
	usecs = (t.tv_sec * 1000000ll) + (t.tv_usec);
	return (usecs);
}
void Time_Pause(long long pause) {
	struct timeval tv;
	tv.tv_sec  = (long long)pause / 1000000;
	tv.tv_usec = (long long)pause % 1000000;
	select(0, NULL, NULL, NULL, &tv);
}
#endif // if WIN32
