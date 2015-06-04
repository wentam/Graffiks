/// \file

#ifndef GFKS_OBJECT_H
#define GFKS_OBJECT_H
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

float gfks_get_object_x(gfks_object *o);
float gfks_get_object_y(gfks_object *o);
float gfks_get_object_z(gfks_object *o);

float gfks_get_object_angle(gfks_object *o);
float gfks_get_object_angle_x(gfks_object *o);
float gfks_get_object_angle_y(gfks_object *o);
float gfks_get_object_angle_z(gfks_object *o);

void gfks_set_object_location(gfks_object *o, float x, float y, float z);

void gfks_set_object_rotation(gfks_object *o, float angle, float x, float y, float z);

// there must be an equal number of materials and meshes.
DLL_EXPORT gfks_object *gfks_create_object(gfks_mesh **meshes, gfks_material **mats,
                                           int mesh_count);
DLL_EXPORT void gfks_remove_object(gfks_object *o);
DLL_EXPORT void gfks_show_object(gfks_object *o);
DLL_EXPORT void gfks_hide_object(gfks_object *o);

#endif
