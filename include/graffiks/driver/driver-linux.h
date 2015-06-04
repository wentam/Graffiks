/// \file

#ifndef GFKS_DRIVER_LINUX_H
#define GFKS_DRIVER_LINUX_H
#include <GL/glx.h>

Display *display;
Window win;

void gfks_init_xorg(int window_width, int window_height, char *window_title,
                    void (*init)(int *width, int *height),
                    void (*update)(float time_step), void (*finish)(void));

void gfks_use_vsync(int vsync);

void gfks_set_antialiasing_samples(int samples);

#endif
