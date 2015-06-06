#include "graffiks/camera.h"
#include "graffiks/gl_helper.h"
#include <stdio.h>
#include <stdlib.h>

gfks_camera *_gfks_active_camera = NULL;

gfks_camera *gfks_create_camera() {
  gfks_camera *c = malloc(sizeof(gfks_camera));
  // camera position and angle
  c->location_x = 0;
  c->location_y = 0;
  c->location_z = 0;
  c->target_x = 0;
  c->target_y = 0;
  c->target_z = 0;
  c->up_x = 0;
  c->up_y = 1;
  c->up_z = 0;

  return c;
}

void gfks_destroy_camera(gfks_camera *c) { free(c); }

void gfks_set_active_camera(gfks_camera *c) { _gfks_active_camera = c; }

// setters
void gfks_set_camera_location(gfks_camera *c, float x, float y, float z) {
  c->location_x = x;
  c->location_y = y;
  c->location_z = z;
}

void gfks_set_camera_target(gfks_camera *c, float x, float y, float z) {
  c->target_x = x;
  c->target_y = y;
  c->target_z = z;
}

void gfks_rotate_camera(gfks_camera *c, float x, float y, float z, float w) {
  float result[3];
  result[0] = c->target_x;
  result[1] = c->target_y;
  result[2] = c->target_z;

  gfks_rotate_vector_by_quaternion(result, x, y, z, w);

  c->target_x = result[0];
  c->target_y = result[1];
  c->target_z = result[2];
}

gfks_camera *gfks_get_active_camera() { return _gfks_active_camera; }

float gfks_get_camera_location_x(gfks_camera *c) { return c->location_x; }
float gfks_get_camera_location_y(gfks_camera *c) { return c->location_y; }
float gfks_get_camera_location_z(gfks_camera *c) { return c->location_z; }

float gfks_get_camera_target_x(gfks_camera *c) { return c->target_x; }
float gfks_get_camera_target_y(gfks_camera *c) { return c->target_y; }
float gfks_get_camera_target_z(gfks_camera *c) { return c->target_z; }

void _gfks_update_view_matrix() {
  if (_gfks_active_camera == NULL) {
    fprintf(stderr, "\x1B[31mGraffiks error: No active camera set. Set one with "
                    "gfks_set_active_camera(gfks_camera *c) in camera.h\n\x1B[0m");
    exit(-1);
  }

  // set up view matrix
  gfks_set_view_matrix(gfks_view_matrix, _gfks_active_camera->location_x,
                       _gfks_active_camera->location_y, _gfks_active_camera->location_z,
                       _gfks_active_camera->target_x, _gfks_active_camera->target_y,
                       _gfks_active_camera->target_z, _gfks_active_camera->up_x,
                       _gfks_active_camera->up_y, _gfks_active_camera->up_z);
}
