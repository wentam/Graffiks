#ifndef OBJECT_H
#define OBJECT_H
#include <stdlib.h>
#include "graffiks/mesh/mesh.h"

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

typedef struct mat gfks_material;

typedef struct {
  gfks_mesh **meshes;
  gfks_material **mats;
  int mesh_count;
  float location_x;
  float location_y;
  float location_z;
  float angle;
  float rot_x;
  float rot_y;
  float rot_z;
} gfks_object;

// there must be an equal number of materials and meshes.
DLL_EXPORT gfks_object *gfks_create_object(gfks_mesh **meshes, gfks_material **mats,
                                           int mesh_count);
DLL_EXPORT void gfks_remove_object(gfks_object *o);
DLL_EXPORT void gfks_show_object(gfks_object *o);
DLL_EXPORT void gfks_hide_object(gfks_object *o);

#endif
