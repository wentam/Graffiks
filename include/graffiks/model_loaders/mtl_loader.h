/// \file

#ifndef GFKS_MTL_LOADER_H
#define GFKS_MTL_LOADER_H

#include "graffiks/material.h"

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

typedef struct {
  gfks_material *mat;
  char *name;
} gfks_named_material;

typedef struct {
  gfks_named_material **mats;
  int number_of_mats;
} gfks_named_material_array;

DLL_EXPORT gfks_named_material_array *gfks_load_mtl(gfks_renderer_flags flags,
                                                    char *filepath);

// does not free the materials contained within
DLL_EXPORT void gfks_free_named_mats(gfks_named_material_array *n);

#endif
