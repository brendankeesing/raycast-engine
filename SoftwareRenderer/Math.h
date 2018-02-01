#ifndef _MATH_H_
#define _MATH_H_

#include <math.h>

#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef TAU
#define TAU (3.14159265359f * 2)
#endif


template <class T>
inline T Clamp(T v, T a, T b)
{
	return v < a ? a : (v > b ? b : v);
}

inline float DegreesToRadians(float v)
{
	return v * 0.0174533f;
}

inline float RadiansToDegrees(float v)
{
	return v * 57.2958f;
}

inline int FloorToInt(float v)
{
	return (int)(v + 32768.0f) - 32768;/*
	int i = int(v);
	if (i > v)
		return i - 1;
	return i;*/
}

struct Vector2
{
	float x, y;

	Vector2() = default;
	Vector2(float x, float y) : x(x), y(y) { }

	inline float GetMagnitudeSqr()
	{
		return x * x + y * y;
	}

	inline float GetMagnitude()
	{
		return sqrtf(GetMagnitudeSqr());
	}

	inline void Normalize()
	{
		float m = 1.0f / GetMagnitude();
		x *= m;
		y *= m;
	}

	inline void Scale(float v)
	{
		x *= v;
		y *= v;
	}

	inline void Add(Vector2 v)
	{
		x += v.x;
		y += v.y;
	}

	inline Vector2 Rotate(float radians)
	{
		return Vector2(
			x * cosf(radians) - y * sinf(radians),
			x * sinf(radians) + y * cosf(radians)
			);
	}

	inline static float Dot(Vector2 a, Vector2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline static float Det(Vector2 a, Vector2 b)
	{
		return a.x * b.y - a.y * b.x;
	}

	inline static float AngleClockwise(Vector2 a, Vector2 b)
	{
		return atan2f(Det(a, b), Dot(a, b));
	}

	inline static Vector2 Lerp(Vector2 a, Vector2 b, float t)
	{
		return Vector2(
			t * (b.x - a.x) + a.x,
			t * (b.y - a.y) + a.y
		);
	}
};

struct Vector2i
{
	int x, y;

	Vector2i() = default;
	Vector2i(int x, int y) : x(x), y(y) { }

	inline int GetMagnitudeSqr()
	{
		return x * x + y * y;
	}

	inline float GetMagnitude()
	{
		return sqrtf((float)GetMagnitudeSqr());
	}

	inline static int Dot(Vector2i a, Vector2i b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline static int Det(Vector2i a, Vector2i b)
	{
		return a.x * b.y - a.y * b.x;
	}
};

struct Vector3
{
	float x, y, z;

	Vector3() = default;
	Vector3(float x, float y, float z) : x(x), y(y), z(z) { }

	inline static float Dot(Vector3 a, Vector3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float GetMagnitudeSqr()
	{
		return x * x + y * y + z * z;
	}

	inline float GetMagnitude()
	{
		return sqrtf(GetMagnitudeSqr());
	}

	inline void Normalize()
	{
		float m = 1.0f / GetMagnitude();
		x *= m;
		y *= m;
		z *= m;
	}
};

inline Vector2 AngleToDirection(float angle)
{
	return Vector2(sinf(angle), cosf(angle));
}

bool GetRayToLineSegmentIntersection(Vector2 rayorigin, Vector2 raydirection, Vector2 p1, Vector2 p2, float* distance)
{
	Vector2 ortho(-raydirection.y, raydirection.x);
	Vector2 p1toorigin(rayorigin.x - p1.x, rayorigin.y - p1.y);
	Vector2 p1top2(p2.x - p1.x, p2.y - p1.y);

	float denom = p1top2.x * ortho.x + p1top2.y * ortho.y;
	if (fabsf(denom) < 0.00001f)
		return false;

	float crosslength = p1top2.x * p1toorigin.y - p1top2.y * p1toorigin.x;
	float t1 = crosslength / denom;
	if (t1 < 0)
		return false;
	float t2 = (p1toorigin.x * ortho.x + p1toorigin.y * ortho.y) / denom;

	*distance = t1;
	return t2 >= 0 && t2 <= 1;
}

float Lerp(float a, float b, float t)
{
	return t * (b - a) + a;
}

float InverseLerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}

bool IsOnRight(Vector2 p0, Vector2 p1, Vector2 p)
{
	return (p1.x - p0.x) * (p.y - p0.y) - (p1.y - p0.y) * (p.x - p0.x) > 0;
}

#endif
