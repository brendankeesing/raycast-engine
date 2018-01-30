#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "Types.h"
#include "Math.h"

namespace Random
{
	sint32 seed = 0;

	inline sint32 Int32()
	{
		seed = (214013 * seed + 2531011);
		return (seed >> 16) & 0x7FFF;
	}

	inline float Range(float min, float max)
	{
		return Lerp(min, max, (float)(Int32() % 10000) / 10000.0f);
	}
}

#endif
