#include "graffiks/internal.h"

static void gfks_free_defaults(gfks_defaults *defaults) {
  defaults->rasterization_settings->free(defaults->rasterization_settings);
  free(defaults);
}

gfks_defaults* init_struct() {
  gfks_defaults* new = malloc(sizeof(gfks_defaults));
  new->rasterization_settings = NULL;

  new->free = &gfks_free_defaults;

  return new;
}

gfks_defaults* gfks_create_defaults() {
  gfks_defaults* new = init_struct();    
  new->rasterization_settings = gfks_create_rasterization_settings();

  return new;
}
