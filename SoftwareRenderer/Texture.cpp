#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>

// REMOVE ME!!!!!
#define RGBA8(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))
#define RGB8(r, g, b) RGBA8(r, g, b, 255)

static int ReadInt(FILE* file, char* buffer)
{
	char* temp;
	fscanf(file, "%s", buffer); // P3
	return strtol(buffer, &temp, 10);
}

Texture* Texture_LoadPPM(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (!file)
		return NULL;

	char buffer[8];

	fscanf(file, "%s", buffer); // P3
	int w = ReadInt(file, buffer);
	int h = ReadInt(file, buffer);
	ReadInt(file, buffer); // 255
	Texture* tex = new Texture(w, h);
	for (int i = 0; i < tex->pixelCount; ++i)
		tex->data[i] = RGB8(ReadInt(file, buffer), ReadInt(file, buffer), ReadInt(file, buffer));

	fclose(file);

	return tex;
}