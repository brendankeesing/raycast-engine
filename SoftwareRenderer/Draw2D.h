#ifndef _DRAW2D_H_
#define _DRAW2D_H_

#include "Math.h"
#include "Draw.h"
#include "Structs.h"

namespace Draw2D
{
	Vector2i Transform2D(DrawData* drawdata, Vector2 p, float zoom)
	{
		return Vector2i(
			(int)((p.x - drawdata->camera->position.x) * zoom) + drawdata->width / 2,
			drawdata->height - ((int)((p.y - drawdata->camera->position.y) * zoom) + drawdata->height / 2)
		);
	}

	void Draw(DrawData* drawdata)
	{
		Draw::FillColor(drawdata->colorBuffer, 0, drawdata->width * drawdata->height, RGB8(0, 0, 0));

		float zoom = (float)drawdata->width / 30;

		// draw walls
		int wallcolor = RGB8(150, 150, 150);
		int sectorwallcolor = RGB8(200, 0, 0);
		for (int s = 0; s < drawdata->scene->sectorCount; ++s)
		{
			Sector* sector = &drawdata->scene->sectors[s];
			for (int w = 0; w < sector->wallCount; ++w)
			{
				Wall* wall = &drawdata->scene->walls[sector->startWall + w];
				Wall* wall2 = &drawdata->scene->walls[sector->startWall + ((w + 1) % sector->wallCount)];

				Vector2 p1 = drawdata->scene->vertices[wall->vertex];
				Vector2 p2 = drawdata->scene->vertices[wall2->vertex];

				Draw::DrawLine(drawdata->colorBuffer, drawdata->width, drawdata->height, Transform2D(drawdata, p1, zoom), Transform2D(drawdata, p2, zoom), wall->nextSector == -1 ? wallcolor : sectorwallcolor);
			}
		}

		// draw player
		int playercolor = RGB8(0, 255, 0);
		Vector2i ppos = Transform2D(drawdata, drawdata->camera->position, zoom);
		Vector2i pforward = Transform2D(drawdata, Vector2(drawdata->camera->position.x + drawdata->camera->forward.x, drawdata->camera->position.y + drawdata->camera->forward.y), zoom);
		Vector2i pright = Transform2D(drawdata, Vector2(drawdata->camera->position.x + drawdata->camera->right.x * 0.5f, drawdata->camera->position.y + drawdata->camera->right.y * 0.5f), zoom);
		Vector2i pleft = Vector2i(ppos.x - pright.x + ppos.x, ppos.y - pright.y + ppos.y);

		Draw::DrawLine(drawdata->colorBuffer, drawdata->width, drawdata->height, ppos, pforward, playercolor);
		Draw::DrawLine(drawdata->colorBuffer, drawdata->width, drawdata->height, pright, pleft, playercolor);
	}
}

#endif