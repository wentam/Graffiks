#include "graffiks/governor.h"
#include "graffiks/graffiks.h"
#include <windows.h>

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

HDC hdc;

DLL_EXPORT void gfks_init_windows(int window_width, int window_height, char *window_title,
                                  void (*init)(int *width, int *height),
                                  void (*update)(float time_step), void (*finish)(void),
                                  HINSTANCE hInstance);

void gfks_use_vsync(int vsync);

void gfks_set_antialiasing_samples(int samples);
