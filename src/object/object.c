#include "object/object.h"
#include <stdlib.h>

object* create_object(mesh **meshes, material **mats, int mesh_count) {
    object *o = malloc(sizeof(object));

    o->meshes = meshes;
    o->mesh_count = mesh_count;
    o->mats = mats;
    o->location_x = 0;
    o->location_y = 0;
    o->location_z = 0;
    o->angle = 0;
    o->rot_x = 0;
    o->rot_y = 0;
    o->rot_z = 1;

    return o;
}

void free_object(object *o) {
    int i;
    for (i = 0; i < o->mesh_count; i++) {
        free_mesh(o->meshes[i]);
    }

    for (i = 0; i < o->mesh_count; i++) {
        free_material(o->mats[i]);
    }

    free(o);
}
