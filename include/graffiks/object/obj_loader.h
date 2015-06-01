#include "graffiks/mesh/mesh.h"
#include <stdio.h>
#include <string.h>
#include "graffiks/object/object.h"
#include "graffiks/material/material.h"
#include "graffiks/renderer/renderer.h"

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

DLL_EXPORT object *load_obj(renderer_flags flags, char *filepath);
