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
		return sqrtf(GetMagnitudeSqr());
	}

	inline static int Dot(Vector2 a, Vector2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline static int Det(Vector2 a, Vector2 b)
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
/*
int GetRayToLineSegmentIntersection2(Vector2 origin, Vector2 dir, Vector2 vert1, Vector2 vert2, Vector2 *normal, Vector2 *normalrot, float *dist, Vector2 *poi)
{
	float d, dp1, dp2;

	d = Vector2::Dot(dir, normal);
	if (d == 0.0f)
		return 0;

	*dist = Vector2::Dot(vert1, normal) - Vector2::Dot(origin, normal);

	if ((*dist <= 0.0f && d > 0.0f) ||
		(*dist > 0.0f && d <= 0.0f))
		return 0;

	*dist /= d;
	*poi = Vector2(origin.x + dir.x * *dist, origin.y + dir.y * *dist);

	d = Vector2::Dot(*poi, normalrot);
	dp1 = Vector2::Dot(vert1, normalrot);
	dp2 = Vector2::Dot(vert2, normalrot);

	if ((d >= dp1 && d <= dp2) || (d <= dp1 && d >= dp2))
		return 1;
	return 0;
}*/

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
