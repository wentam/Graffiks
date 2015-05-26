#include "material/material.h"

material *create_material(renderer_flags flags) {
  float *diffuse_color = malloc(sizeof(float) * 4);
  diffuse_color[0] = 1;
  diffuse_color[1] = 1;
  diffuse_color[2] = 1;
  diffuse_color[3] = 1;

  material *m = malloc(sizeof(material));
  m->diffuse_intensity = 10;
  m->diffuse_color = diffuse_color;
  m->renderer = flags;

  return m;
}

void free_material(material *m) {
  free(m->diffuse_color);
  free(m);
}

void set_diffuse_intensity(material *m, float intensity) {
  m->diffuse_intensity = intensity;
}

void set_diffuse_color(material *m, float diffuse_color[]) {
  float *dc = malloc(sizeof(float) * 4);
  int i;
  for (i = 0; i < 4; i++) {
    dc[i] = diffuse_color[i];
  }
  m->diffuse_color = dc;
}
