#include "graffiks/object/object.h"
#include <stdlib.h>
#include "graffiks/material/material.h"
#include "graffiks/renderer/renderer.h"

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

  show_object(o);

  return o;
}

void show_object(object *o) {
  // add all meshes to render_queue
  // TODO: Showing an object twice in a row puts it in the queue twice. This shouldn't
  // happen.
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    render_queue_size++;
    render_queue = realloc(render_queue, sizeof(render_queue_item *) * render_queue_size);

    render_queue[render_queue_size - 1] = malloc(sizeof(render_queue_item));
    render_queue[render_queue_size - 1]->mesh = o->meshes[i];
    render_queue[render_queue_size - 1]->material = o->mats[i];
    render_queue[render_queue_size - 1]->parent_object = o;
  }
}

void hide_object(object *o) {
  // remove the object's meshes/materials from the render queue
  int i;
  int left_offset = 0;
  for (i = 0; i < render_queue_size; i++) {
    render_queue[i - left_offset] = render_queue[i];

    if (render_queue[i]->parent_object == o) {
      free(render_queue[i]);
      left_offset++; // every element from this point on needs to be offset
                     // once more than last time
    }
  }

  render_queue_size -= left_offset;

  render_queue = realloc(render_queue, sizeof(render_queue_item) * render_queue_size);

  if (render_queue_size == 0) {
    free(render_queue);
  }
}

void remove_object(object *o) {
  hide_object(o);

  int i;
  for (i = 0; i < o->mesh_count; i++) {
    free_mesh(o->meshes[i]);
  }
  free(o->meshes);

  for (i = 0; i < o->mesh_count; i++) {
    free_material(o->mats[i]);
  }

  free(o);
}
