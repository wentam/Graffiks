#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H
#include "graffiks/object/object.h"

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

DLL_EXPORT void _init_renderer_fw();
DLL_EXPORT void _draw_from_queue_fw();
DLL_EXPORT void _draw_object_fw(object *o);
DLL_EXPORT void _terminate_renderer_fw();
DLL_EXPORT void _clear_fw();

#endif
