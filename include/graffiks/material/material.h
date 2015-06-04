#ifndef GFKS_MATERIAL_H
#define GFKS_MATERIAL_H
#include "graffiks/renderer/renderer.h"

#ifndef DLL_EXPORT
#ifdef _WIN32
#ifdef GRAFFIKS_BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif
#endif

typedef struct mat {
  float *diffuse_color;
  float specularity_hardness;
  float specularity_color_r;
  float specularity_color_g;
  float specularity_color_b;
  gfks_renderer_flags renderer;
} gfks_material;

DLL_EXPORT gfks_material *gfks_create_material(gfks_renderer_flags flags);
DLL_EXPORT void gfks_free_material(gfks_material *m);

void gfks_set_material_diffuse_color_rgba(gfks_material *m, float r, float g, float b,
                                          float a);
void gfks_set_material_specularity_hardness(gfks_material *m, float hardness);
void gfks_set_material_specularity_rgb(gfks_material *m, float r, float g, float b);

float gfks_get_material_specularity_hardness(gfks_material *m);
float gfks_get_material_specularity_color_r(gfks_material *m);
float gfks_get_material_specularity_color_g(gfks_material *m);
float gfks_get_material_specularity_color_b(gfks_material *m);

float gfks_get_material_diffuse_color_r(gfks_material *m);
float gfks_get_material_diffuse_color_g(gfks_material *m);
float gfks_get_material_diffuse_color_b(gfks_material *m);
float gfks_get_material_diffuse_color_a(gfks_material *m);
#endif
