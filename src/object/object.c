#include "graffiks/object/object.h"
#include <stdlib.h>
#include "graffiks/material/material.h"
#include "graffiks/renderer/renderer.h"

gfks_object *gfks_create_object(gfks_mesh **meshes, gfks_material **mats,
                                int mesh_count) {
  gfks_object *o = malloc(sizeof(gfks_object));

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

  gfks_show_object(o);

  return o;
}

void gfks_show_object(gfks_object *o) {
  // add all meshes to render_queue
  // TODO: Showing an object twice in a row puts it in the queue twice. This shouldn't
  // happen.
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    gfks_render_queue_size++;
    gfks_render_queue = realloc(gfks_render_queue, sizeof(gfks_render_queue_item *) *
                                                       gfks_render_queue_size);

    gfks_render_queue[gfks_render_queue_size - 1] =
        malloc(sizeof(gfks_render_queue_item));
    gfks_render_queue[gfks_render_queue_size - 1]->mesh = o->meshes[i];
    gfks_render_queue[gfks_render_queue_size - 1]->material = o->mats[i];
    gfks_render_queue[gfks_render_queue_size - 1]->parent_object = o;
  }
}

void gfks_hide_object(gfks_object *o) {
  // remove the object's meshes/materials from the render queue
  int i;
  int left_offset = 0;
  for (i = 0; i < gfks_render_queue_size; i++) {
    gfks_render_queue[i - left_offset] = gfks_render_queue[i];

    if (gfks_render_queue[i]->parent_object == o) {
      free(gfks_render_queue[i]);
      left_offset++; // every element from this point on needs to be offset
                     // once more than last time
    }
  }

  gfks_render_queue_size -= left_offset;

  gfks_render_queue =
      realloc(gfks_render_queue, sizeof(gfks_render_queue_item) * gfks_render_queue_size);

  if (gfks_render_queue_size == 0) {
    free(gfks_render_queue);
  }
}

void gfks_remove_object(gfks_object *o) {
  gfks_hide_object(o);

  int i;
  for (i = 0; i < o->mesh_count; i++) {
    gfks_free_mesh(o->meshes[i]);
  }
  free(o->meshes);

  for (i = 0; i < o->mesh_count; i++) {
    gfks_free_material(o->mats[i]);
  }

  free(o);
}
