#ifndef GRAFFIKS_H
#define GRAFFIKS_H
#ifdef ANDROID
#include <android/log.h>
#endif

// To use the engine, call graffiks_setup with functions pointers for init, update, and
// draw
// See other header files to see what functions are avaiable for drawing.
void graffiks_setup(void (*init)(int *width, int *height),
                    void (*update)(float time_step), void (*finish)(void));

void set_draw_callback(void (*draw)(void));

// internal stuff
void _call_init(int *width, int *height);
void _call_update(float time_step);
void _call_draw();
void _call_finish();
#endif
