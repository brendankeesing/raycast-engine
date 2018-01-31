#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include "Math.h"
#include "Scene.h"

struct Camera
{
	Vector2 position = { 1, 1 };
	float height = 0;
	float angle = DegreesToRadians(0);
	float focalLength = 1.0f;

	// calculated
	Vector2 forward = { 0, 1 };
	Vector2 right = { 1, 0 };

	void CalculateDirections()
	{
		forward = AngleToDirection(angle);
		right = AngleToDirection(angle + DegreesToRadians(90));
	}
};

struct Player : Camera
{
	Vector3 velocity = { 0, 0, 0 };
	float angleVelocity = 0;
} defaultPlayer;

struct DrawData
{
	Camera* camera = &defaultPlayer;
	Scene* scene = &defaultScene;
	int width = 1200;
	int height = 800;
	int* colorBuffer = NULL;
	bool draw3D = true;
} drawData;

#endif