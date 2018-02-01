#ifndef _DRAW3D_H_
#define _DRAW3D_H_

#include "Structs.h"
#include "Draw.h"

namespace Draw3D
{
	struct ColumnData
	{
		int x;
		Vector2 rayDirection;
		float localAngle;
		float worldAngle;
		float distanceToProjectionPlane;
	};

	void DrawFloor(DrawData* drawdata, ColumnData* columndata, bool isfloor, int ystart, int yend, float floorheight, Texture* tex, Vector2 textureoffset, Vector2 texturescale)
	{
		float tanfov = drawdata->camera->focalLength;
		float pixeltogradcoefficent = tanfov * 2.0f / drawdata->width;

		float dirx = columndata->rayDirection.x * floorheight;
		float diry = columndata->rayDirection.y * floorheight;

		float distortionfix = cosf(columndata->localAngle) * pixeltogradcoefficent; // remove distortion
		for (int y = ystart; y < yend; y++)
		{
			float dist;
			if (isfloor)
				dist = 1.0f / ((y - drawdata->height / 2) * distortionfix);
			else
				dist = 1.0f / ((drawdata->height / 2 - y) * distortionfix);

			float worldx = textureoffset.x + texturescale.x * (drawdata->camera->position.x + dirx * dist);
			float worldy = textureoffset.y + texturescale.y * (drawdata->camera->position.y + diry * dist);

			int texx = FloorToInt(worldx * tex->width) % tex->width;
			if (texx < 0)
				texx += tex->width;
			int texy = FloorToInt(worldy * tex->height) % tex->height;
			if (texy < 0)
				texy += tex->height;

			drawdata->colorBuffer[y * drawdata->width + columndata->x] = tex->data[texy * tex->width + texx];
		}
	}

	void DrawWall(DrawData* drawdata, int x, int ystart, int yend, int yoffscreenstart, int yoffscreenend, float wallheight, float dist, Texture* tex, Vector2 textureoffset, Vector2 texturescale, float texturepoint, bool checkalpha = false)
	{
		// get texture info
		int texx = FloorToInt(tex->width * (textureoffset.x + texturepoint * texturescale.x)) % tex->width;
		if (texx < 0)
			texx += tex->width;
		float mul = texturescale.y * wallheight / (float)(yoffscreenend - yoffscreenstart);
		for (int y = ystart; y < yend; ++y)
		{
			int texy = FloorToInt((float)tex->height * (textureoffset.y + (float)(y - yoffscreenstart) * mul));
			texy %= tex->height;
			if (texy < 0)
				texy += tex->height;

			int color = tex->data[texx + texy * tex->width];
			if (!checkalpha || GetA(color) > 128)
				drawdata->colorBuffer[y * drawdata->width + x] = color;
		}
	}

	void DrawSky(DrawData* drawdata, int x, int ystart, int yend, float angle, Texture* tex)
	{
		angle = fmodf(angle, TAU);
		if (angle < 0)
			angle = angle + TAU;
		int texx = (int)((angle / PI) * tex->width);
		texx %= tex->width;

		for (int y = ystart; y < yend; ++y)
		{
			int texy = (y * tex->height) / drawdata->height;
			drawdata->colorBuffer[y * drawdata->width + x] = tex->data[texy * tex->width + texx];
		}
	}

	void DrawColumn(DrawData* drawdata, ColumnData* columndata, int ystart, int yend, Sector* sector);

	void DrawSector(DrawData* drawdata, ColumnData* columndata, int ystart, int yend, Sector* sector, float dist)
	{
		// calculate wall pixel height on screen
		float wallheight = sector->ceilingHeight - sector->floorHeight;
		int lineheight = (int)(columndata->distanceToProjectionPlane * wallheight / dist);

		// find where the wall will be drawn
		float walloffset = drawdata->camera->height - (sector->floorHeight + sector->ceilingHeight);
		int yoffset = (int)(0.5f * columndata->distanceToProjectionPlane * walloffset / dist);
		int yoffscreenstart = (drawdata->height - lineheight) / 2 + yoffset;
		int yoffscreenend = yoffscreenstart + lineheight;
		int ywallstart = Clamp(yoffscreenstart, ystart, yend);
		int ywallend = Clamp(yoffscreenend, ystart, yend);

		DrawColumn(drawdata, columndata, ywallstart, ywallend, sector);
	}

	void DrawColumn(DrawData* drawdata, ColumnData* columndata, int ystart, int yend, Sector* sector)
	{
		// find raycast hit point
		float dist;
		float texturepoint;
		Wall* wall = RaycastWallsFromInside(drawdata->scene, sector, drawdata->camera->position, columndata->rayDirection, &dist, &texturepoint);
		if (wall == NULL)
			return;
		dist *= cosf(columndata->localAngle); // remove distortion

		texturepoint = fmodf(texturepoint, 1.0f); // wrap texture

		// calculate wall pixel height on screen
		float wallheight = sector->ceilingHeight - sector->floorHeight;
		int lineheight = (int)(columndata->distanceToProjectionPlane * wallheight / dist);

		// find where the wall will be drawn
		float walloffset = drawdata->camera->height - (sector->floorHeight + sector->ceilingHeight);
		int yoffset = (int)(0.5f * columndata->distanceToProjectionPlane * walloffset / dist);
		int yoffscreenstart = (drawdata->height - lineheight) / 2 + yoffset;
		int yoffscreenend = yoffscreenstart + lineheight;
		int ywallstart = Clamp(yoffscreenstart, ystart, yend);
		int ywallend = Clamp(yoffscreenend, ystart, yend);

		// floor
		float floorheight = fabsf(sector->floorHeight * 2 - drawdata->camera->height);
		if (sector->floorSurface.sky)
			DrawSky(drawdata, columndata->x, ywallend, yend, columndata->worldAngle, drawdata->scene->textures[sector->floorSurface.textureID]);
		else
			DrawFloor(drawdata, columndata, true, ywallend, yend, floorheight, drawdata->scene->textures[sector->floorSurface.textureID], sector->floorSurface.textureOffset, sector->floorSurface.textureScale);

		// ceiling
		float ceilingheight = fabsf(sector->ceilingHeight * 2 - drawdata->camera->height);
		if (sector->ceilingSurface.sky)
			DrawSky(drawdata, columndata->x, ystart, ywallstart, columndata->worldAngle, drawdata->scene->textures[sector->ceilingSurface.textureID]);
		else
			DrawFloor(drawdata, columndata, false, ystart, ywallstart, ceilingheight, drawdata->scene->textures[sector->ceilingSurface.textureID], sector->ceilingSurface.textureOffset, sector->ceilingSurface.textureScale);

		// draw wall
		if (wall->surface.sky)
			DrawSky(drawdata, columndata->x, ywallstart, ywallend, columndata->worldAngle, drawdata->scene->textures[wall->surface.textureID]);
		else
			DrawWall(drawdata, columndata->x, ywallstart, ywallend, yoffscreenstart, yoffscreenend, wallheight, dist, drawdata->scene->textures[wall->surface.textureID], wall->surface.textureOffset, wall->surface.textureScale, texturepoint);

		// OPTIMISE: This could be better because it's draw the entire wall before drawing the sector when only part of the wall needs to be drawn
		if (wall->flags & WallFlags::ConnectSector)
		{
			DrawSector(drawdata, columndata, ywallstart, ywallend, &drawdata->scene->sectors[wall->nextSector], dist);
			if (wall->flags & WallFlags::DrawSectorWall)
				DrawWall(drawdata, columndata->x, ywallstart, ywallend, yoffscreenstart, yoffscreenend, wallheight, dist, drawdata->scene->textures[wall->surface.textureID], wall->surface.textureOffset, wall->surface.textureScale, texturepoint, true);
		}
	}

	void Draw(DrawData* drawdata)
	{
		Sector* sector = GetSectorAtPoint(drawdata->scene, drawdata->camera->position);
		if (sector == NULL)
			return;

		ColumnData columndata;
		columndata.distanceToProjectionPlane = drawdata->width / drawdata->camera->focalLength;

		for (columndata.x = 0; columndata.x < drawdata->width; ++columndata.x)
		{
			// calculate viewport
			float projectionplanedist = drawdata->camera->focalLength * ((float)columndata.x / drawdata->width - 0.5f);
			columndata.rayDirection = Vector2(
				drawdata->camera->forward.x + drawdata->camera->right.x * projectionplanedist,
				drawdata->camera->forward.y + drawdata->camera->right.y * projectionplanedist
			);
			columndata.rayDirection.Normalize();
			columndata.localAngle = -Vector2::AngleClockwise(drawdata->camera->forward, columndata.rayDirection);
			columndata.worldAngle = defaultPlayer.angle + columndata.localAngle;

			DrawColumn(drawdata, &columndata, 0, drawdata->height, sector);
		}
	}
}

#endif