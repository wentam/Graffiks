#ifndef MATERIAL_H
#define MATERIAL_H
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef ANDROID
#include <GLES2/gl2.h>
#include "jni.h"
#include <android/asset_manager_jni.h>
#include <android/log.h>
#endif

#ifdef LINUX
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif

#include "renderer/renderer.h"

typedef struct mat {
  GLuint *program;
  float diffuse_intensity;
  float *diffuse_color;
  renderer_flags renderer;
} material;

material *create_material(renderer_flags flags);
void free_material(material *m);
void set_diffuse_intensity(material *m, float intensity);
void set_diffuse_color(material *m, float diffuse_color[]);

GLuint *_create_program(char *vertex_shader_filepath, char *fragment_shader_filepath);

#endif
