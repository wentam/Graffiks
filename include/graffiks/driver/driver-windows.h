#include "graffiks/governor.h"
#include "graffiks/graffiks.h"
#include <windows.h>
#include <GL/glew.h>
#include <GL/GL.h>

void init_graffiks_windows(int window_width, int window_height, char *window_title,
                        void (*init)(int *width, int *height),
                        void (*update)(float time_step), void (*finish)(void),
                        HINSTANCE hInstance);

void use_vsync(int vsync);

void set_antialiasing_samples(int samples);
