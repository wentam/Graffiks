#ifndef GOVERNOR_H
#define GOVERNOR_H

#include <stdlib.h>
#include "graffiks/gl_helper.h"
#include "graffiks/graffiks.h"

#ifndef _WIN32
#include <sys/time.h>
#include "graffiks/driver/driver-linux.h"
#endif

#ifdef _WIN32
#include "graffiks/driver/driver-windows.h"
#endif

#ifdef ANDROID
#include <android/log.h>
#include <GLES2/gl2.h>
#endif

#include <math.h>

#ifndef DLL_EXPORT
# ifdef _WIN32
#  ifdef GRAFFIKS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# else
#  define DLL_EXPORT
# endif
#endif

int renderer_width;
int renderer_height;

// put the camera where you want it
DLL_EXPORT void set_camera_location_target_and_up(float x, float y, float z, float tx, float ty,
                                       float tz, float ux, float uy, float uz);

// to be controlled by a driver such as driver-jni-android.c
void _init_graffiks();
void _set_size();
void _draw_frame();
void _finish();

// other internal stuff
float view_matrix[16];
float view_matrix_inverse[16];
float projection_matrix[16];
void _limit_fps(int fps);
void _ms(long long int *ms);
void _sleep_ms(int ms);
#endif
