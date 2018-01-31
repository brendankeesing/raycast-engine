#include <cstdio>
#include "Display.h"
#include "Draw2D.h"
#include "Draw3D.h"

char title_text[128];

void on_window_resize(int w, int h)
{
	if (drawData.colorBuffer)
		delete[] drawData.colorBuffer;

	drawData.width = w;
	drawData.height = h;
	drawData.colorBuffer = new int[w * h];
}

void on_input(unsigned char key, bool pressed)
{
	float movespeed = 2;
	float heightspeed = 0.5f;
	float rotspeed = DegreesToRadians(60);
	switch (key)
	{
	case 'q':
		defaultPlayer.velocity.x = pressed ? -movespeed : 0;
		break;
	case 'e':
		defaultPlayer.velocity.x = pressed ? movespeed : 0;
		break;
	case 's':
		defaultPlayer.velocity.y = pressed ? -movespeed : 0;
		break;
	case 'w':
		defaultPlayer.velocity.y = pressed ? movespeed : 0;
		break;
	case 'a':
		defaultPlayer.angleVelocity = pressed ? -rotspeed : 0;
		break;
	case 'd':
		defaultPlayer.angleVelocity = pressed ? rotspeed : 0;
		break;
	case 'z':
		defaultPlayer.velocity.z = pressed ? -heightspeed : 0;
		break;
	case 'x':
		defaultPlayer.velocity.z = pressed ? heightspeed : 0;
		break;
	case 'r':
		if (pressed)
			drawData.draw3D = !drawData.draw3D;
		break;
	}
}

void on_update(float deltatime)
{
	// update player movement
	defaultPlayer.angle += defaultPlayer.angleVelocity * deltatime;
	defaultPlayer.CalculateDirections();
	Vector2 offset =
	{
		defaultPlayer.forward.x * defaultPlayer.velocity.y + defaultPlayer.right.x * defaultPlayer.velocity.x,
		defaultPlayer.forward.y * defaultPlayer.velocity.y + defaultPlayer.right.y * defaultPlayer.velocity.x
	};
	defaultPlayer.position.x += offset.x * deltatime;
	defaultPlayer.position.y += offset.y * deltatime;
	defaultPlayer.height += defaultPlayer.velocity.z * deltatime;

	// show FPS in the window title
	float fps = 1.0f / deltatime;
	sprintf(title_text, "Software Renderer %dx%d FPS: %.2f", drawData.width, drawData.height, fps);
	display_set_window_title(title_text);

	if (drawData.draw3D)
		Draw3D::Draw(&drawData);
	else
		Draw2D::Draw(&drawData);

	display_set_image(drawData.width, drawData.height, drawData.colorBuffer);
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
	on_window_resize(drawData.width, drawData.height);

	display_init(&argc, argv, drawData.width, drawData.height, "Software Renderer");
	display_begin_loop(on_update, on_window_resize, on_input);

	for (int i = 0; i < drawData.scene->textureCount; ++i)
		delete drawData.scene->textures[i];
	delete[] drawData.colorBuffer;

	return 0;
}