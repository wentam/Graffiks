#ifndef GRAFFIKS_H
#define GRAFFIKS_H
#ifdef ANDROID
#include <android/log.h>
#endif

#ifndef DLL_EXPORT
#ifdef _WIN32
#ifdef GRAFFIKS_BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif
#endif

// To use the engine, call graffiks_setup with functions pointers for init, update, and
// draw
// See other header files to see what functions are avaiable for drawing.
DLL_EXPORT void gfks_setup(void (*init)(int *width, int *height),
                           void (*update)(float time_step), void (*finish)(void));

DLL_EXPORT void gfks_set_draw_callback(void (*draw)(void));

// internal stuff
void _gfks_call_init(int *width, int *height);
void _gfks_call_update(float time_step);
void _gfks_call_draw();
void _gfks_call_finish();
#endif
