#include "Texture.h"
#include "Color.h"
#include <stdio.h>
#include <stdlib.h>

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
	bool hasalpha = buffer[1] == '4';
	int w = ReadInt(file, buffer);
	int h = ReadInt(file, buffer);
	ReadInt(file, buffer); // 255
	Texture* tex = new Texture(w, h, hasalpha);

	if (hasalpha)
	{
		for (int i = 0; i < tex->pixelCount; ++i)
			tex->data[i] = RGBA8(ReadInt(file, buffer), ReadInt(file, buffer), ReadInt(file, buffer), ReadInt(file, buffer));
	}
	else
	{
		for (int i = 0; i < tex->pixelCount; ++i)
			tex->data[i] = RGB8(ReadInt(file, buffer), ReadInt(file, buffer), ReadInt(file, buffer));
	}

	fclose(file);

	return tex;
}