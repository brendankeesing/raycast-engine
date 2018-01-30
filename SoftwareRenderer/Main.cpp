#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Display.h"
#include "Types.h"
#include "Random.h"
#include "Math.h"
#include "Texture.h"
#include "Scene.h"
#include "Color.h"

//////////////////// DRAWING ////////////////////

int* color_buffer = NULL;
int screen_width = 600;
int screen_height = 400;

void fill_color(int start, int size, int color)
{
	size += start;
	for (int i = start; i < size; ++i)
		color_buffer[i] = color;
}

void draw_quad(int x, int y, int w, int h, int color)
{
	w = Clamp(x + w, 0, screen_width - 1);
	h = Clamp(y + h, 0, screen_height - 1);
	x = Clamp(x, 0, screen_width - 1);
	y = Clamp(y, 0, screen_height - 1);

	for (; y < h; ++y)
	{
		for (int xx = x; xx < w; ++xx)
			color_buffer[y * screen_width + xx] = color;
	}
}

void draw_quad(float x, float y, float w, float h, int color)
{
	x += 0.5f - w * 0.5f;
	y += 0.5f + h * -0.5f;
	int width = (int)(screen_width * w);
	int height = (int)(screen_height * h);
	draw_quad((int)(screen_width * x), (int)(screen_height * y), width, height, color);
}

void draw_texture(int screenx, int screeny, int texx, int texy, int width, int height, int texwidth, int* texture)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
			color_buffer[(screeny + y) * screen_width + x + screenx] = texture[(texy + y) * texwidth + x + texx];
	}
}

void cpy(int* src, int* dst, int size)
{
	int* end = src + size;
	while (src != end)
	{
		*dst = *src;
		++src;
		++dst;
	}
}

void draw_texture_fast(int x, int y, int width, int height, int* texture)
{
	int screenystart = y * screen_width + x;
	int texyend = width * height;

	for (int texystart = 0; texystart < texyend; texystart += width)
	{
		screenystart += screen_width;
		memcpy(color_buffer + screenystart, texture + texystart, width * sizeof(int));
	}
}


//////////////////// MAIN ////////////////////

struct Player
{
	// camera
	Vector2 position = { 1, 1 };
	float height = 0;
	float angle = DegreesToRadians(0);
	float focalLength = 1.0f;

	// calculated
	Vector2 forward = { 0, 1 };
	Vector2 right = { 1, 0 };

	// movement
	Vector3 velocity = { 0, 0, 0 };
	float angleVelocity = 0;
} player;


//////////////////// OTHER ////////////////////

char title_text[128];
bool draw3D = true;

void on_window_resize(int w, int h)
{
	if (color_buffer)
		delete[] color_buffer;

	screen_width = w;
	screen_height = h;

	color_buffer = new int[screen_width * screen_height];
}

void on_input(unsigned char key, bool pressed)
{
	float movespeed = 2;
	float heightspeed = 0.5f;
	float rotspeed = DegreesToRadians(60);
	switch (key)
	{
	case 'q':
		player.velocity.x = pressed ? -movespeed : 0;
		break;
	case 'e':
		player.velocity.x = pressed ? movespeed : 0;
		break;
	case 's':
		player.velocity.y = pressed ? -movespeed : 0;
		break;
	case 'w':
		player.velocity.y = pressed ? movespeed : 0;
		break;
	case 'a':
		player.angleVelocity = pressed ? -rotspeed : 0;
		break;
	case 'd':
		player.angleVelocity = pressed ? rotspeed : 0;
		break;
	case 'z':
		player.velocity.z = pressed ? -heightspeed : 0;
		break;
	case 'x':
		player.velocity.z = pressed ? heightspeed : 0;
		break;
	case 'r':
		if (pressed)
			draw3D = !draw3D;
		break;
	}
}

void DrawLine(int x0, int y0, int x1, int y1, int color)
{
	int dx = abs(x1 - x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		if (x0 >= 0 && x0 < screen_width && y0 >= 0 && y0 < screen_height) // this is uber crap!!!
			color_buffer[y0 * screen_width + x0] = color;

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

void DrawLine(Vector2i p0, Vector2i p1, int color)
{
	DrawLine(p0.x, p0.y, p1.x, p1.y, color);
}

Vector2i Transform2D(Vector2 p, float zoom)
{
	return Vector2i(
		(p.x - player.position.x) * zoom + screen_width / 2,
		p.y = screen_height - ((p.y - player.position.y) * zoom + screen_height / 2)
		);
}

void Draw2D()
{
	fill_color(0, screen_width * screen_height, RGB8(0, 0, 0));

	float zoom = screen_width / 30;

	// draw walls
	int wallcolor = RGB8(150, 150, 150);
	int sectorwallcolor = RGB8(200, 0, 0);
	for (int s = 0; s < scene.sectorCount; ++s)
	{
		Sector* sector = &scene.sectors[s];
		for (int w = 0; w < sector->wallCount; ++w)
		{
			Wall* wall = &scene.walls[sector->startWall + w];
			Wall* wall2 = &scene.walls[sector->startWall + ((w + 1) % sector->wallCount)];

			Vector2 p1 = scene.vertices[wall->vertex];
			Vector2 p2 = scene.vertices[wall2->vertex];

			DrawLine(Transform2D(p1, zoom), Transform2D(p2, zoom), wall->nextSector == -1 ? wallcolor : sectorwallcolor);
		}
	}

	// draw player
	int playercolor = RGB8(0, 255, 0);
	Vector2i ppos = Transform2D(player.position, zoom);
	Vector2i pforward = Transform2D(Vector2(player.position.x + player.forward.x, player.position.y + player.forward.y), zoom);
	Vector2i pright = Transform2D(Vector2(player.position.x + player.right.x * 0.5f, player.position.y + player.right.y * 0.5f), zoom);
	Vector2i pleft = Vector2i(ppos.x - pright.x + ppos.x, ppos.y - pright.y + ppos.y);

	DrawLine(ppos, pforward, playercolor);
	DrawLine(pright, pleft, playercolor);
}

int ApplyDistanceShadow(int color, float dist)
{
	float mindist = 8;
	if (dist < mindist)
		return color;

	return RGB8(0, 0, 0);// test

	float maxdist = 15;
	if (dist > maxdist)
		return RGB8(0, 0, 0);

	dist = Clamp(InverseLerp(maxdist, mindist, dist), 0.0f, 1.0f);
	int r = (int)(GetR(color) * dist);
	int g = (int)(GetG(color) * dist);
	int b = (int)(GetB(color) * dist);
	return RGB8(r, g, b);
}

Vector3 PixelToDirection(int x, int y)
{
	float projectionplanedistx = player.focalLength * ((float)x / screen_width - 0.5f);
	float projectionplanedisty = player.focalLength * ((float)y / screen_height - 0.5f);
	Vector3 raydir(
		player.forward.x + player.right.x * projectionplanedistx,
		player.forward.y + player.right.y * projectionplanedistx,
		projectionplanedisty
	);
	raydir.Normalize();
	return raydir;
}

Vector2 Lerp(Vector2 a, Vector2 b, float t)
{
	return Vector2(
		t * (b.x - a.x) + a.x,
		t * (b.y - a.y) + a.y
	);
}

struct ColumnData
{
	int x;
	Vector2 rayDirection;
	float localAngle;
	float worldAngle;
	float distanceToProjectionPlane;
};

void DrawFloor(ColumnData* columndata, bool isfloor, int ystart, int yend, float floorheight, Texture* tex)
{
	float tanfov = player.focalLength;
	float pixeltogradcoefficent = tanfov * 2.0f / screen_width;

	float dirx = columndata->rayDirection.x * floorheight;
	float diry = columndata->rayDirection.y * floorheight;

	float distortionfix = cosf(columndata->localAngle) * pixeltogradcoefficent; // remove distortion
	for (int y = ystart; y < yend; y++)
	{
		float invdist;
		if (isfloor)
			invdist = (y - screen_height / 2) * distortionfix;
		else
			invdist = (screen_height / 2 - y) * distortionfix;
		//float dist = 1.0f / invdist;

		float worldx = player.position.x + dirx / invdist;
		float worldy = player.position.y + diry / invdist;

		int texx = (int)(worldx * tex->width) % tex->width;
		if (texx < 0)
			texx += tex->width;
		int texy = (int)(worldy * tex->height) % tex->height;
		if (texy < 0)
			texy += tex->height;

		int color = tex->data[texy * tex->width + texx];
		//dist = Vector2(player.position.x - world.x, player.position.y - world.y).GetMagnitude();
		//color = ApplyDistanceShadow(color, dist);
		color_buffer[y * screen_width + columndata->x] = color;
	}
}

void DrawWall(int x, int ystart, int yend, int yoffscreenstart, int yoffscreenend, float dist, Texture* tex, float texturepoint)
{
	// get texture info
	int lineheight = yoffscreenend - yoffscreenstart;
	float texoffset = (float)tex->height / lineheight;
	float currenttexy = (ystart - yoffscreenstart) * texoffset;
	int texstart = (int)(tex->width * texturepoint);

	for (int y = ystart; y < yend; ++y)
	{
		int color = tex->data[texstart + (int)currenttexy * tex->width];
		//color = ApplyDistanceShadow(color, dist);
		color_buffer[y * screen_width + x] = color;
		currenttexy += texoffset;
	}
}

void DrawSky(int x, int ystart, int yend, float angle, Texture* tex)
{
	angle = fmodf(angle, TAU);
	if (angle < 0)
		angle = angle + TAU;
	int texx = (angle / PI) * tex->width;
	texx %= tex->width;

	for (int y = ystart; y < yend; ++y)
	{
		int texy = (y * tex->height) / screen_height;
		color_buffer[y * screen_width + x] = tex->data[texy * tex->width + texx];
	}
}

void DrawColumn(ColumnData* columndata, int ystart, int yend, Sector* sector);

void DrawSector(ColumnData* columndata, int ystart, int yend, Sector* sector, float dist)
{
	// calculate wall pixel height on screen
	float wallheight = sector->ceilingHeight - sector->floorHeight;
	int lineheight = (int)(columndata->distanceToProjectionPlane * wallheight / dist);

	// find where the wall will be drawn
	float walloffset = player.height - (sector->floorHeight + sector->ceilingHeight);
	int yoffset = 0.5f * columndata->distanceToProjectionPlane * walloffset / dist;
	int yoffscreenstart = (screen_height - lineheight) / 2 + yoffset;
	int yoffscreenend = yoffscreenstart + lineheight;
	int ywallstart = Clamp(yoffscreenstart, ystart, yend);
	int ywallend = Clamp(yoffscreenend, ystart, yend);

	DrawColumn(columndata, ywallstart, ywallend, sector);
}

void DrawColumn(ColumnData* columndata, int ystart, int yend, Sector* sector)
{
	// find raycast hit point
	float dist;
	float texturepoint;
	Wall* wall = RaycastWallsFromInside(&scene, sector, player.position, columndata->rayDirection, &dist, &texturepoint);
	if (wall == NULL)
		return;
	dist *= cosf(columndata->localAngle); // remove distortion

	texturepoint = fmodf(texturepoint, 1.0f); // wrap texture

	// calculate wall pixel height on screen
	float wallheight = sector->ceilingHeight - sector->floorHeight;
	int lineheight = (int)(columndata->distanceToProjectionPlane * wallheight / dist);

	// find where the wall will be drawn
	float walloffset = player.height - (sector->floorHeight + sector->ceilingHeight);
	int yoffset = 0.5f * columndata->distanceToProjectionPlane * walloffset / dist;
	int yoffscreenstart = (screen_height - lineheight) / 2 + yoffset;
	int yoffscreenend = yoffscreenstart + lineheight;
	int ywallstart = Clamp(yoffscreenstart, ystart, yend);
	int ywallend = Clamp(yoffscreenend, ystart, yend);

	// floor
	float floorheight = fabsf(sector->floorHeight * 2 - player.height);
	if (sector->floorSurface.sky)
		DrawSky(columndata->x, ywallend, yend, columndata->worldAngle, scene.textures[sector->floorSurface.textureID]);
	else
		DrawFloor(columndata, true, ywallend, yend, floorheight, scene.textures[sector->floorSurface.textureID]);

	// ceiling
	float ceilingheight = fabsf(sector->ceilingHeight * 2 - player.height);
	if (sector->ceilingSurface.sky)
		DrawSky(columndata->x, ystart, ywallstart, columndata->worldAngle, scene.textures[sector->ceilingSurface.textureID]);
	else
		DrawFloor(columndata, false, ystart, ywallstart, ceilingheight, scene.textures[sector->ceilingSurface.textureID]);

	// draw wall
	if (wall->surface.sky)
		DrawSky(columndata->x, ywallstart, ywallend, columndata->worldAngle, scene.textures[wall->surface.textureID]);
	else
		DrawWall(columndata->x, ywallstart, ywallend, yoffscreenstart, yoffscreenend, dist, scene.textures[wall->surface.textureID], texturepoint);
	if (wall->nextSector != -1)
		DrawSector(columndata, ywallstart, ywallend, &scene.sectors[wall->nextSector], dist);
}

void Draw3D()
{
	Sector* sector = GetSectorAtPoint(&scene, player.position);
	if (sector == NULL)
		return;

	ColumnData columndata;
	columndata.distanceToProjectionPlane = screen_width / player.focalLength;

	for (columndata.x = 0; columndata.x < screen_width; ++columndata.x)
	{
		// calculate viewport
		float projectionplanedist = player.focalLength * ((float)columndata.x / screen_width - 0.5f);
		columndata.rayDirection = Vector2(
			player.forward.x + player.right.x * projectionplanedist,
			player.forward.y + player.right.y * projectionplanedist
		);
		columndata.rayDirection.Normalize();
		columndata.localAngle = -Vector2::AngleClockwise(player.forward, columndata.rayDirection);
		columndata.worldAngle = player.angle + columndata.localAngle;

		DrawColumn(&columndata, 0, screen_height, sector);
	}
}

void on_update(float deltatime)
{
	// update player movement
	player.angle += player.angleVelocity * deltatime;
	player.forward = AngleToDirection(player.angle);
	player.right = AngleToDirection(player.angle + DegreesToRadians(90));
	Vector2 offset =
	{
		player.forward.x * player.velocity.y + player.right.x * player.velocity.x,
		player.forward.y * player.velocity.y + player.right.y * player.velocity.x
	};
	player.position.x += offset.x * deltatime;
	player.position.y += offset.y * deltatime;
	player.height += player.velocity.z * deltatime;

	// show FPS in the window title
	float fps = 1.0f / deltatime;
	sprintf(title_text, "Software Renderer %dx%d FPS: %.2f", screen_width, screen_height, fps);
	display_set_window_title(title_text);

	if (draw3D)
		Draw3D();
	else
		Draw2D();

	display_set_image(screen_width, screen_height, color_buffer);
}

#include "Timer.h"
void ProfileTest()
{
	int tests = 100000000;

	Vector2 test(1, 2);
	Timer timer;
	timer.Reset();

	short test1 = 0;
	for (int i = 0; i < tests; ++i)
	{
		test1 += 3;
	}

	float timeA = timer.GetSeconds();
	printf("Test 1: %f\n", timeA);
	timer.Reset();

	char test2 = 0;
	for (int i = 0; i < tests; ++i)
	{
		test2 += 3;
	}

	float timeB = timer.GetSeconds();
	printf("Test 2: %f\n", timeB);
	printf("Difference: %f%%\n", 100.0f * timeB / timeA);
}

int main(int argc, char *argv[])
{
	//ProfileTest();

	// setup texture
	on_window_resize(screen_width, screen_height);

	display_init(&argc, argv, screen_width * 2, screen_height * 2, "Software Renderer");
	display_begin_loop(on_update, on_window_resize, on_input);

	for (int i = 0; i < scene.textureCount; ++i)
		delete scene.textures[i];
	delete[] color_buffer;

	return 0;
}