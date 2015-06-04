#ifndef MATERIAL_H
#define MATERIAL_H
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
DLL_EXPORT void gfks_set_diffuse_color(gfks_material *m, float diffuse_color[]);

#endif
