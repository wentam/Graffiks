/// \file

#ifndef GFKS_RENDERER_H
#define GFKS_RENDERER_H
#include "graffiks/object.h"

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

/// \cond INTERNAL
int renderer_width;
int renderer_height;

extern gfks_renderer_flags gfks_enabled_renderers;
float gfks_projection_matrix[16];

typedef struct {
  gfks_mesh *mesh;
  gfks_material *material;
  gfks_object *parent_object;
} gfks_render_queue_item;

extern gfks_render_queue_item **gfks_render_queue;
extern int gfks_render_queue_size;
/// \endcond

DLL_EXPORT void gfks_init_renderers(gfks_renderer_flags flags);
DLL_EXPORT void gfks_terminate_renderers(gfks_renderer_flags flags);

/// \cond INTERNAL
DLL_EXPORT void gfks_set_renderer_size(int width, int height);
DLL_EXPORT void gfks_draw_objects();
DLL_EXPORT void gfks_clear(gfks_renderer_flags flags);
DLL_EXPORT GLuint *_gfks_create_program(char *vertex_shader_filepath,
                                        char *fragment_shader_filepath);
/// \endcond

#endif
