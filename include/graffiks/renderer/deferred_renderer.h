#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H
#include <GL/gl.h>
#include "graffiks/object/object.h"
#include "graffiks/lights.h"

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

DLL_EXPORT void _init_renderer_df();
DLL_EXPORT void _draw_object_df(object *o);
DLL_EXPORT void _terminate_renderer_df();
DLL_EXPORT void _clear_df();
DLL_EXPORT void _ambient_pass_df();
DLL_EXPORT void _light_pass_point_df(point_light *light);
DLL_EXPORT void _light_pass_df();
DLL_EXPORT void _geom_pass_df();

#endif
