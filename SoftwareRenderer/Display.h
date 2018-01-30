#ifndef _DISPLAY_H_
#define _DISPLAY_H_

void display_init(int* argcp, char** argv, int w, int h, const char* title);
void display_begin_loop(void (*onupdate)(float deltatime), void (*onresize)(int w, int h), void (*oninput)(unsigned char key, bool pressed));
void display_set_image(int w, int h, void* data);
void display_set_target_fps(float fps);
void display_set_window_size(int w, int h);
void display_set_window_title(const char* title);

#endif