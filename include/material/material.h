#ifndef MATERIAL_H
#define MATERIAL_H
#include "renderer/renderer.h"

typedef struct mat {
  float diffuse_intensity;
  float *diffuse_color;
  renderer_flags renderer;
} material;

material *create_material(renderer_flags flags);
void free_material(material *m);
void set_diffuse_intensity(material *m, float intensity);
void set_diffuse_color(material *m, float diffuse_color[]);

#endif
