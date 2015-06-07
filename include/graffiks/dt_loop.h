#ifndef GFKS_GOVERNOR_H
#define GFKS_GOVERNOR_H

#include <stdlib.h>
#include "graffiks/gl_helper.h"
#include "graffiks/dt_callbacks.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

#include "graffiks/graffiks.h"

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

// to be controlled by a driver such as driver-linux.c
void _gfks_dt_start_loop();
void _gfks_dt_finish();

#endif
