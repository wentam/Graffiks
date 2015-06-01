#ifndef RENDERER_H
#define RENDERER_H
#include "graffiks/object/object.h"
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

typedef enum {
  GRAFFIKS_RENDERER_DEFERRED = 0x01,
  GRAFFIKS_RENDERER_FORWARD = 0x02
} renderer_flags;

extern renderer_flags enabled_renderers;

typedef struct {
  mesh *mesh;
  material *material;
  object *parent_object;
} render_queue_item;

extern render_queue_item **render_queue;
extern int render_queue_size;

DLL_EXPORT void init_renderers(renderer_flags flags);
DLL_EXPORT void terminate_renderers(renderer_flags flags);
DLL_EXPORT void _clear(renderer_flags flags);
DLL_EXPORT GLuint *_create_program(char *vertex_shader_filepath, char *fragment_shader_filepath);

#endif
