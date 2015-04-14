#include "object.h"
#include <stdlib.h>


object* create_object(mesh **meshes, material **mats, int mesh_count) {
    object *o = malloc(sizeof(object));

    o->meshes = meshes;
    o->mesh_count = mesh_count;
    o->mats = mats;

    return o;
}

void draw_object(object *o) {
    int i;
    for (i = 0; i < o->mesh_count; i++) {
        draw_mesh(o->meshes[i],o->mats[i]);
    }
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
