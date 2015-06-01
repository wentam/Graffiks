#include "graffiks/material/material.h"

#ifndef DLL_EXPORT
# ifdef _WIN32
#  ifdef GRAFFIKS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# else
#  define DLL_EXPORT
# endif
#endif

typedef struct {
  material *mat;
  char *name;
} named_material;

typedef struct {
  named_material **mats;
  int number_of_mats;
} named_material_array;

DLL_EXPORT named_material_array *load_mtl(renderer_flags flags, char *filepath);

// does not free the materials contained within
DLL_EXPORT void free_named_mats(named_material_array *n);
