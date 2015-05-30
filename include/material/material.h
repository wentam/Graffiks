#ifndef MATERIAL_H
#define MATERIAL_H
#include "renderer/renderer.h"

typedef struct mat {
  float *diffuse_color;
  float specularity_hardness;
  float specularity_color_r;
  float specularity_color_g;
  float specularity_color_b;
  renderer_flags renderer;
} material;

material *create_material(renderer_flags flags);
void free_material(material *m);
void set_diffuse_color(material *m, float diffuse_color[]);

#endif
