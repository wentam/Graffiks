#ifndef MATERIAL_H
#define MATERIAL_H
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>
#include <GLES2/gl2.h>

#ifdef ANDROID
#include "jni.h"
#include <android/asset_manager_jni.h>
#include <android/log.h>
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
