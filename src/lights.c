#include "graffiks/lights.h"
#include <stdlib.h>

const int GFKS_LIGHT_ATTENUATION_LINEAR = 0x01;
const int GFKS_LIGHT_ATTENUATION_QUADRATIC = 0x02;
const int GFKS_LIGHT_ATTENUATION_CONSTANT = 0x04;
int gfks_point_light_count = 0;
float gfks_ambient_color[] = {0, 0, 0};

gfks_point_light *gfks_add_point_light() {
  gfks_point_lights = realloc(gfks_point_lights,
                              sizeof(gfks_point_light *) * (gfks_point_light_count + 1));
  gfks_point_light *l = malloc(sizeof(gfks_point_light));
  l->x = 0;
  l->y = 0;
  l->z = 0;
  l->brightness = 10;
  l->attenuation_mode = GFKS_LIGHT_ATTENUATION_LINEAR;
  l->attenuation_strength = 1;

  gfks_point_lights[gfks_point_light_count++] = l;

  return l;
}

float gfks_get_point_light_x(gfks_point_light *l) { return l->x; }
float gfks_get_point_light_y(gfks_point_light *l) { return l->y; }
float gfks_get_point_light_z(gfks_point_light *l) { return l->z; }
float gfks_get_point_light_brightness(gfks_point_light *l) { return l->brightness; }

void gfks_set_point_light_location(gfks_point_light *l, float x, float y, float z) {
  l->x = x;
  l->y = y;
  l->z = z;
}

void gfks_set_point_light_brightness(gfks_point_light *l, float brightness) {
  l->brightness = brightness;
}

void gfks_set_ambient_light(float r, float g, float b) {
  gfks_ambient_color[0] = r;
  gfks_ambient_color[1] = g;
  gfks_ambient_color[2] = b;
}

void gfks_remove_point_light(gfks_point_light *light) {
  // shift all elements to the right of the light once to the left
  bool past_light = false;
  int i;
  for (i = 0; i < gfks_point_light_count; i++) {
    if (past_light) {
      gfks_point_lights[i - 1] = gfks_point_lights[i];
    }

    if (gfks_point_lights[i] == light) {
      past_light = true;
    }
  }

  gfks_point_light_count--;

  // realloc the array to the new (smaller) size
  gfks_point_lights =
      realloc(gfks_point_lights, sizeof(gfks_point_light *) * (gfks_point_light_count));

  // if this was the last light, free the array
  if (gfks_point_light_count == 0) {
    free(gfks_point_lights);
  }

  // free the light
  free(light);
}
