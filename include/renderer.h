#ifndef RENDERER_H
#define RENDERER_H
#include <GLES2/gl2.h>
#include <sys/time.h>
#include <stdlib.h>
#include "gl_helper.h"
#include "graffiks.h"
#include "driver-linux.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#include <math.h>

// change this to change the ambient color: {r,g,b,intensity}
extern float ambient_color[4];

// put the camera where you want it :)
void set_camera_location_target_and_up(float x, float y, float z,
                                       float tx, float ty, float tz,
                                       float ux, float uy, float uz);

// to be controlled by a driver such as driver-jni-android.c
void _init_graffiks();
void _set_size();
void _draw_frame();
void _finish();

// other internal stuff
float view_matrix[16];
float projection_matrix[16];
void _limit_fps(int fps);
void _ms(long long int *ms);
void _sleep_ms(int ms);
#endif
