#ifndef MATERIAL_H
#define MATERIAL_H
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>

#ifdef ANDROID
#include <GLES2/gl2.h>
#include "jni.h"
#include <android/asset_manager_jni.h>
#include <android/log.h>
#endif

#ifdef LINUX
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif

typedef struct {
    GLuint *program;
    float diffuse_intensity;
    float *diffuse_color;
} material;

material* create_material();
void free_material(material *m);
void set_diffuse_intensity(material *m, float intensity);
void set_diffuse_color(material *m, float diffuse_color[]);
#endif
