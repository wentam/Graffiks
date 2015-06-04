#ifndef GFKS_DRIVER_LINUX_H
#define GFKS_DRIVER_LINUX_H

#include "graffiks/governor.h"
#include "graffiks/graffiks.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

Display *display;
Window win;

void gfks_init_xorg(int window_width, int window_height, char *window_title,
                    void (*init)(int *width, int *height),
                    void (*update)(float time_step), void (*finish)(void));

void gfks_use_vsync(int vsync);

void gfks_set_antialiasing_samples(int samples);

#endif
