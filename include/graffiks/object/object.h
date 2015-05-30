#ifndef OBJECT_H
#define OBJECT_H
#include <stdlib.h>
#include "graffiks/mesh/mesh.h"

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
object *create_object(mesh **meshes, material **mats, int mesh_count);
void remove_object(object *o);
void show_object(object *o);
void hide_object(object *o);

#endif
