#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture
{
public:
	Texture(int w, int h, bool hasalpha) : width(w), height(h), hasAlpha(hasalpha), pixelCount(w * h), data(new int[pixelCount]) {}
	~Texture()
	{
		delete[] data;
	}

	const int width;
	const int height;
	const int pixelCount;
	const bool hasAlpha;
	int* const data;
};

Texture* Texture_LoadPPM(const char* filename);

#endif

