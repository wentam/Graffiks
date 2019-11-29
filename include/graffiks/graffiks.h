/// \file

#ifndef GFKS_DRIVER_H
#define GFKS_DRIVER_H

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

#ifdef _WIN32
#include "graffiks/dt_loop.h"
#include "graffiks/dt_callbacks.h"
#include <windows.h>
#endif

#ifdef LINUX
#include <GL/glx.h>
#endif

#ifdef _WIN32
HDC hdc;
#endif

#ifdef LINUX
Display *display;
Window win;
#endif

DLL_EXPORT void gfks_init_dt(int window_width, int window_height, char *window_title,
                             void (*init)(int *width, int *height),
                             void (*update)(float time_step), void (*finish)(void));

#ifdef LINUX
DLL_EXPORT void gfks_init_with_window(Display *display, Window window);
DLL_EXPORT void gfks_init_with_window_dt(Display *display, Window window,
                             void (*init)(int *width, int *height),
                             void (*update)(float time_step), void (*finish)(void));
#endif

#ifdef _WIN32
// windows version of init_with_window and init_with_window_dt
#endif

DLL_EXPORT void gfks_init(int window_width, int window_height, char *window_title);

DLL_EXPORT void gfks_use_vsync(int vsync);

DLL_EXPORT void gfks_set_antialiasing_samples(int samples);

#endif
