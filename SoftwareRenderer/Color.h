#ifndef _COLOR_H_
#define _COLOR_H_

#define RGBA8(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))
#define RGB8(r, g, b) RGBA8(r, g, b, 255)

#define GetR(c) (c & 0x000000ff)
#define GetG(c) ((c & 0x0000ff00) >> 8)
#define GetB(c) ((c & 0x00ff0000) >> 16)
#define GetA(c) ((c & 0xff000000) >> 24)

#endif
