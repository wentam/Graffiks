#include "governor.h"
#include "graffiks.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>



Display *display;
Window win;

void init_graffiks_xorg(int window_width, int window_height, char *window_title,
        void (*init)(int *width, int *height),
        void (*update)(float time_step),
        void (*draw)(void),
        void (*finish)(void));

void use_vsync(int vsync);

void set_antialiasing_samples (int samples);
