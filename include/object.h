#include "mesh.h"
#include "material.h"

#ifndef OBJECT_H
#define OBJECT_H

typedef struct {
   mesh **meshes;
   material **mats;
   int mesh_count;
} object;

// there must be an equal number of materials and meshes.
object* create_object(mesh **meshes, material **mats, int mesh_count);
void draw_object(object *o);
void free_object(object *o);

#endif
