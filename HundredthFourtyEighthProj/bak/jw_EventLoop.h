#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "uv.h"
#include "jw_Singleton.h"

class EventLoop : public Singleton<EventLoop>
{
public:
	void init();

	int run();
	void stop();
	void exit();
	uv_loop_t* getLoop();

private:
	uv_loop_t* mLoop;
};


#endif