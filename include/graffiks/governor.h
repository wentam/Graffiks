#ifndef GFKS_GOVERNOR_H
#define GFKS_GOVERNOR_H

#include <stdlib.h>
#include "graffiks/gl_helper.h"
#include "graffiks/dt_callbacks.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

#include "graffiks/driver.h"

#ifdef ANDROID
#include <android/log.h>
#include <GLES2/gl2.h>
#endif

#include <math.h>

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

// put the camera where you want it
DLL_EXPORT void gfks_set_camera_location_target_and_up(float x, float y, float z,
                                                       float tx, float ty, float tz,
                                                       float ux, float uy, float uz);

// to be controlled by a driver such as driver-jni-android.c
void _gfks_init();
void _gfks_draw_frame();
void _gfks_finish();

// other internal stuff
void _gfks_limit_fps(int fps);
void _gfks_ms(long long int *ms);
void _gfks_sleep_ms(int ms);
#endif
