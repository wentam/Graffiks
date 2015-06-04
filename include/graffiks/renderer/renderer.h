#ifndef GFKS_RENDERER_H
#define GFKS_RENDERER_H
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

typedef enum {
  GFKS_RENDERER_DEFERRED = 0x01,
  GFKS_RENDERER_FORWARD = 0x02
} gfks_renderer_flags;

extern gfks_renderer_flags gfks_enabled_renderers;

typedef struct {
  gfks_mesh *mesh;
  gfks_material *material;
  gfks_object *parent_object;
} gfks_render_queue_item;

extern gfks_render_queue_item **gfks_render_queue;
extern int gfks_render_queue_size;

DLL_EXPORT void gfks_init_renderers(gfks_renderer_flags flags);
DLL_EXPORT void gfks_terminate_renderers(gfks_renderer_flags flags);
DLL_EXPORT void _gfks_clear(gfks_renderer_flags flags);
DLL_EXPORT GLuint *_gfks_create_program(char *vertex_shader_filepath,
                                        char *fragment_shader_filepath);

#endif
