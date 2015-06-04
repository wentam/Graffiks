#include "graffiks/material/material.h"

gfks_material *gfks_create_material(gfks_renderer_flags flags) {
  float *diffuse_color = malloc(sizeof(float) * 4);
  diffuse_color[0] = 1;
  diffuse_color[1] = 1;
  diffuse_color[2] = 1;
  diffuse_color[3] = 1;

  gfks_material *m = malloc(sizeof(gfks_material));
  m->diffuse_color = diffuse_color;
  m->renderer = flags;
  m->specularity_hardness = 5;
  m->specularity_color_r = 0.2;
  m->specularity_color_g = 0.2;
  m->specularity_color_b = 0.2;

  return m;
}

void gfks_free_material(gfks_material *m) {
  free(m->diffuse_color);
  free(m);
}

void gfks_set_diffuse_color(gfks_material *m, float diffuse_color[]) {
  float *dc = malloc(sizeof(float) * 4);
  int i;
  for (i = 0; i < 4; i++) {
    dc[i] = diffuse_color[i];
  }
  m->diffuse_color = dc;
}
