#ifndef _TIMER_H_
#define _TIMER_H_

#include "Types.h"

class Timer
{
public:
	Timer();

	float GetSeconds();
	sint64 GetTicks();
	bool Reset();

private:
	sint64 Before;
	sint64 Frequency;
};

#endif