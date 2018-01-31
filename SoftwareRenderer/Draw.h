#ifndef _DRAW_H_
#define _DRAW_H_

#include <cstring>
#include "Math.h"
#include "Texture.h"

namespace Draw
{
	void FillColor(int* buffer, int start, int size, int color)
	{
		size += start;
		for (int i = start; i < size; ++i)
			buffer[i] = color;
	}

	void CopyBuffer(int* src, int* dst, int size)
	{
		int* end = src + size;
		while (src != end)
		{
			*dst = *src;
			++src;
			++dst;
		}
	}

	void DrawQuad(Texture* tex, int x, int y, int w, int h, int color)
	{
		w = Clamp(x + w, 0, tex->width - 1);
		h = Clamp(y + h, 0, tex->height - 1);
		x = Clamp(x, 0, tex->width - 1);
		y = Clamp(y, 0, tex->height - 1);

		int* ptr = tex->data;
		for (; y < h; ++y)
		{
			for (int xx = x; xx < w; ++xx)
				ptr[xx] = color;
			ptr += tex->width;
		}
	}

	void DrawTexture(Texture* tex, int startx, int starty, int texx, int texy, int width, int height, int texwidth, int* texture)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				tex->data[(starty + y) * tex->width + x + startx] = texture[(texy + y) * texwidth + x + texx];
		}
	}

	void DrawTextureFast(Texture* tex, int x, int y, int width, int height, int* texture)
	{
		int screenystart = y * tex->width + x;
		int texyend = width * height;

		for (int texystart = 0; texystart < texyend; texystart += width)
		{
			screenystart += tex->width;
			memcpy(tex->data + screenystart, texture + texystart, width * sizeof(int));
		}
	}

	void DrawLine(int* buffer, int bufferwidth, int bufferheight, int x0, int y0, int x1, int y1, int color)
	{
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = (dx > dy ? dx : -dy) / 2, e2;

		for (;;)
		{
			if (x0 >= 0 && x0 < bufferwidth && y0 >= 0 && y0 < bufferheight) // OPTIMISE: this is uber crap!!!
				buffer[y0 * bufferwidth + x0] = color;

			if (x0 == x1 && y0 == y1)
				break;
			e2 = err;
			if (e2 > -dx)
			{
				err -= dy;
				x0 += sx;
			}
			if (e2 < dy)
			{
				err += dx;
				y0 += sy;
			}
		}
	}

	void DrawLine(int* buffer, int bufferwidth, int bufferheight, Vector2i p0, Vector2i p1, int color)
	{
		DrawLine(buffer, bufferwidth, bufferheight, p0.x, p0.y, p1.x, p1.y, color);
	}
}

#endif