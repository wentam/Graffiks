#include "mesh.h"
#include "material.h"
#include <GLES2/gl2.h>
#include <stdlib.h>
#include "renderer.h"

#ifndef OBJECT_H
#define OBJECT_H

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
object* create_object(mesh **meshes, material **mats, int mesh_count);
void _draw_mesh(object *o, mesh *m, material *mat);
void draw_object(object *o);
void free_object(object *o);

#endif
