#define WIN32_LEAN_AND_MEAN //remove MFC overhead from windows.h which can cause slowness
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Timer.h"

struct Vector2
{
	GLfloat x;
	GLfloat y;
};

struct Vector3
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

static Vector2 screenQuadPositions[] = { { -1, -1 },{ -1, 1 },{ 1, 1 },{ 1, -1 } };
static Vector2 screenQuadUvs[] = { { 0, 1 },{ 0, 0 },{ 1, 0 },{ 1, 1 } };
static GLuint screenTextureID;

static void(*on_update_callback)(float) = NULL;
static void(*on_resize_callback)(int, int) = NULL;
static void(*on_input)(unsigned char, bool) = NULL;
static float target_delta_time = 60;
static Timer timer;

static void on_window_resize(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (on_resize_callback)
		on_resize_callback(w, h);
}

static void on_idle()
{
	// timer
	while (timer.GetSeconds() < 1.0f / target_delta_time);
	float timedelta = timer.GetSeconds();
	float fps = 1.0f / timedelta;
	timer.Reset();

	// update
	if (on_update_callback)
		on_update_callback(timedelta);

	// force everything to render again
	glutPostRedisplay();
}

static void on_render()
{
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	// draw fullscreen quad
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, screenQuadPositions);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, screenQuadUvs);

	glBindTexture(GL_TEXTURE_2D, screenTextureID);

	glDrawArrays(GL_QUADS, 0, 4);

	// present to screen
	glutSwapBuffers();
}

static void on_keyboard_down(unsigned char key, int, int)
{
	if (on_input)
		on_input(key, true);
}

static void on_keyboard_up(unsigned char key, int, int)
{
	if (on_input)
		on_input(key, false);
}

void display_init(int* argc, char** argv, int w, int h, const char* title)
{
	// initialise window
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(w, h);
	glutCreateWindow("Software Renderer");

	// create screen texture
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// create texture
	glGenTextures(1, &screenTextureID);
	glBindTexture(GL_TEXTURE_2D, screenTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display_begin_loop(void(*onupdate)(float deltatime), void(*onresize)(int w, int h), void(*oninput)(unsigned char key, bool pressed))
{
	on_update_callback = onupdate;
	on_resize_callback = onresize;
	on_input = oninput;

	// setup callbacks
	glutReshapeFunc(on_window_resize);
	glutKeyboardFunc(on_keyboard_down);
	glutKeyboardUpFunc(on_keyboard_up);
	glutIdleFunc(on_idle);
	glutDisplayFunc(on_render);

	glutMainLoop();

	// cleanup
	glDeleteTextures(1, &screenTextureID);
}

void display_set_image(int w, int h, void* data)
{
	glBindTexture(GL_TEXTURE_2D, screenTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display_set_target_fps(float fps)
{
	target_delta_time = 1.0f / fps;
}

void display_set_window_size(int w, int h)
{
	glutInitWindowSize(w, h);
}

void display_set_window_title(const char* title)
{
	glutSetWindowTitle(title);
}
