#ifndef OBJECT_H
#define OBJECT_H
#include <stdlib.h>
#include "graffiks/mesh/mesh.h"

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

typedef struct mat material;

typedef struct {
  mesh **meshes;
  material **mats;
  int mesh_count;
  float location_x;
  float location_y;
  float location_z;
  float angle;
  float rot_x;
  float rot_y;
  float rot_z;
} object;

// there must be an equal number of materials and meshes.
DLL_EXPORT object *create_object(mesh **meshes, material **mats, int mesh_count);
DLL_EXPORT void remove_object(object *o);
DLL_EXPORT void show_object(object *o);
DLL_EXPORT void hide_object(object *o);

#endif
