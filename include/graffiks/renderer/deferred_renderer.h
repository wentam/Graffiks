#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H
#include <GL/gl.h>
#include "graffiks/object/object.h"
#include "graffiks/lights.h"

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

DLL_EXPORT void _gfks_init_renderer_df();
DLL_EXPORT void _gfks_draw_object_df(gfks_object *o);
DLL_EXPORT void _gfks_terminate_renderer_df();
DLL_EXPORT void _gfks_clear_df();
DLL_EXPORT void _gfks_ambient_pass_df();
DLL_EXPORT void _gfks_light_pass_point_df(gfks_point_light *light);
DLL_EXPORT void _gfks_light_pass_df();
DLL_EXPORT void _gfks_geom_pass_df();

#endif
