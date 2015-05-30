#include "graffiks/material/material.h"

typedef struct {
  material *mat;
  char *name;
} named_material;

typedef struct {
  named_material **mats;
  int number_of_mats;
} named_material_array;

named_material_array *load_mtl(renderer_flags flags, char *filepath);

// does not free the materials contained within
void free_named_mats(named_material_array *n);
