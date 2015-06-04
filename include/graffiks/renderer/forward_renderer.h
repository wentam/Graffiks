#ifndef GFKS_FORWARD_RENDERER_H
#define GFKS_FORWARD_RENDERER_H
#include "graffiks/object/object.h"

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

DLL_EXPORT void _gfks_init_renderer_fw();
DLL_EXPORT void _gfks_draw_from_queue_fw();
DLL_EXPORT void _gfks_draw_object_fw(gfks_object *o);
DLL_EXPORT void _gfks_terminate_renderer_fw();
DLL_EXPORT void _gfks_clear_fw();

#endif
