#ifndef _SCENE_H_
#define _SCENE_H_

#include "Math.h"
#include "Color.h"
#include "Texture.h"

struct Surface
{
	int textureID;
	Vector2 textureOffset;
	Vector2 textureScale;
	bool sky;
};

struct Sector
{
	int startWall;
	int wallCount;
	float floorHeight;
	float ceilingHeight;
	Surface floorSurface;
	Surface ceilingSurface;
};

enum WallFlags
{
	None = 0x00,
	ConnectSector = 0x01,
	Collidable = 0x02,
	DrawSectorWall = 0x04,
};

struct Wall
{
	int vertex;
	int nextSector;
	Surface surface;
	WallFlags flags : 8;
};

struct Scene
{
	int sectorCount = 3;
	Sector sectors[3] =
	{
		{ 0, 7, -0.5f, 1.0f, { 6, { 0, 0 }, { 1, 1 }, false }, { 6, { 0, 0 }, { 2, 2 }, false } },
		{ 7, 6, -0.5f, 0.5f, { 6, { 0, 0 }, { 1, 1 }, false }, { 4, { 0, 0 }, { 2, 2 }, false } },
		{ 13, 6, -0.5f, 1.0f, { 3, { 0, 0 }, { 1, 1 }, false }, { 5, { 0, 0 }, { 1, 1 }, true } }
	};

	int wallCount = 19;
	Wall walls[19] =
	{
		{ 0, -1, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 1, 1, { 7, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector | WallFlags::DrawSectorWall },
		{ 2, -1, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 3, -1, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 4, 2, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector },
		{ 5, -1, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 6, -1, { 2, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },

		{ 1, -1, { 1, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 7, -1, { 1, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 8, -1, { 1, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 9, 2, { 1, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector },
		{ 10, -1, { 1, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 2, 0, { 7, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector | WallFlags::DrawSectorWall },

		{ 5, 0, { 0, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector },
		{ 4, -1, { 0, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 11, -1, { 0, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 10, 1, { 0, { 0, 0 }, { 1, 1 }, false }, WallFlags::ConnectSector },
		{ 9, -1, { 0, { 0, 0 }, { 1, 1 }, false }, WallFlags::None },
		{ 12, -1, { 5, { 0, 0 }, { 1, 1 }, true }, WallFlags::None }
	};

	int vertexCount = 13;
	Vector2 vertices[13] =
	{
		{ 0, 0 },
		{ 6, 0 },
		{ 6, 1 },
		{ 6, 5 },
		{ 3, 5 },
		{ 2, 5 },
		{ 0, 5 },

		{ 14, 0 },
		{ 14, 11 },
		{ 6, 11 },
		{ 6, 10 },

		{ 3, 10 },
		{ 2, 11 }
	};

	int textureCount = 8;
	Texture* textures[8] =
	{
		Texture_LoadPPM("Textures\\BrickRed.ppm"),
		Texture_LoadPPM("Textures\\BrickGray.ppm"),
		Texture_LoadPPM("Textures\\Wood.ppm"),
		Texture_LoadPPM("Textures\\Concrete.ppm"),
		Texture_LoadPPM("Textures\\Plaster.ppm"),
		Texture_LoadPPM("Textures\\Sky.ppm"),
		Texture_LoadPPM("Textures\\Grid.ppm"),
		Texture_LoadPPM("Textures\\Bars.ppm")
	};
} defaultScene;

float TexturePointOnWall(Vector2 p1, Vector2 p2, Vector2 intersection)
{
	Vector2 diff(p2.x - p1.x, p2.y - p1.y);
	float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);

	if (fabsf(diff.x) > fabsf(diff.y))
		return InverseLerp(p1.x, p2.x, intersection.x) * dist;
	else
		return InverseLerp(p1.y, p2.y, intersection.y) * dist;
}

Wall* RaycastWallsFromInside(Scene* scene, Sector* sector, Vector2 origin, Vector2 direction, float* dist, float* texturepoint)
{
	Wall* hitwall = NULL;
	*texturepoint = 0;
	*dist = 999999999.0f;

	for (int w = 0; w < sector->wallCount; ++w)
	{
		Wall* wall = &scene->walls[sector->startWall + w];
		Wall* wall2 = &scene->walls[sector->startWall + ((w + 1) % sector->wallCount)];

		Vector2 p1 = scene->vertices[wall->vertex];
		Vector2 p2 = scene->vertices[wall2->vertex];

		// don't draw if on other side
		if (!IsOnRight(p1, p2, origin))
			continue;

		// raycast
		float d;
		if (GetRayToLineSegmentIntersection(origin, direction, p1, p2, &d) && d < *dist)
		{
			hitwall = wall;
			*dist = d;
			Vector2 intersection(origin.x + direction.x * d, origin.y + direction.y * d);
			*texturepoint = TexturePointOnWall(p1, p2, intersection);
		}
	}

	return hitwall;
}

Sector* GetSectorAtPoint(Scene* scene, Vector2 point)
{
	const float maxdist = 1000;
	Vector2 origin(point.x - maxdist, point.y);
	Vector2 direction(1, 0);
	for (int s = 0; s < scene->sectorCount; ++s)
	{
		Sector* sector = &scene->sectors[s];
		int hits = 0;
		for (int w = 0; w < sector->wallCount; ++w)
		{
			Wall* wall = &scene->walls[sector->startWall + w];
			Wall* wall2 = &scene->walls[sector->startWall + ((w + 1) % sector->wallCount)];

			Vector2 p1 = scene->vertices[wall->vertex];
			Vector2 p2 = scene->vertices[wall2->vertex];

			// raycast
			float d;
			if (GetRayToLineSegmentIntersection(origin, direction, p1, p2, &d) && d < maxdist)
				++hits;
		}

		if (hits % 2 == 1)
			return sector;
	}
	return NULL;
}

#endif

