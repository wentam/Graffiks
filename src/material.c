#include "graffiks/material.h"

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

void gfks_set_material_diffuse_color_rgba(gfks_material *m, float r, float g, float b,
                                          float a) {
  m->diffuse_color[0] = r;
  m->diffuse_color[1] = g;
  m->diffuse_color[2] = b;
  m->diffuse_color[3] = a;
}

void gfks_set_material_specularity_hardness(gfks_material *m, float hardness) {
  m->specularity_hardness = hardness;
}

void gfks_set_material_specularity_rgb(gfks_material *m, float r, float g, float b) {
  m->specularity_color_r = r;
  m->specularity_color_g = g;
  m->specularity_color_b = b;
}

float gfks_get_material_specularity_hardness(gfks_material *m) {
  return m->specularity_hardness;
}
float gfks_get_material_specularity_color_r(gfks_material *m) {
  return m->specularity_color_r;
}
float gfks_get_material_specularity_color_g(gfks_material *m) {
  return m->specularity_color_g;
}
float gfks_get_material_specularity_color_b(gfks_material *m) {
  return m->specularity_color_b;
}

float gfks_get_material_diffuse_color_r(gfks_material *m) { return m->diffuse_color[0]; }
float gfks_get_material_diffuse_color_g(gfks_material *m) { return m->diffuse_color[1]; }
float gfks_get_material_diffuse_color_b(gfks_material *m) { return m->diffuse_color[2]; }
float gfks_get_material_diffuse_color_a(gfks_material *m) { return m->diffuse_color[3]; }
