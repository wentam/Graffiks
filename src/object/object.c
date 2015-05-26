#include "object/object.h"
#include <stdlib.h>
#include "material/material.h"
#include "renderer/renderer.h"

object *create_object(mesh **meshes, material **mats, int mesh_count) {
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

  // add all meshes to render_queue
  int i;
  for (i = 0; i < mesh_count; i++) {
    render_queue_size++;
    render_queue = realloc(render_queue, sizeof(render_queue_item *) * render_queue_size);

    render_queue[render_queue_size - 1] = malloc(sizeof(render_queue_item));
    render_queue[render_queue_size - 1]->mesh = meshes[i];
    render_queue[render_queue_size - 1]->material = mats[i];
    render_queue[render_queue_size - 1]->parent_object = o;
  }

  return o;
}

// TODO rename to remove_object and remove all of object's meshes from the queue
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
