#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture
{
public:
	Texture(int w, int h) : width(w), height(h), pixelCount(w * h), data(new int[pixelCount]) {}
	~Texture()
	{
		delete[] data;
	}

	const int width;
	const int height;
	const int pixelCount;
	int* const data;
};

Texture* Texture_LoadPPM(const char* filename);

#endif

