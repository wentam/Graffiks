#include "graffiks/lights.h"
#include <stdlib.h>

const int GRAFFIKS_LIGHT_ATTENUATION_LINEAR = 0x01;
const int GRAFFIKS_LIGHT_ATTENUATION_QUADRATIC = 0x02;
const int GRAFFIKS_LIGHT_ATTENUATION_CONSTANT = 0x04;
int point_light_count = 0;
float ambient_color[] = {0, 0, 0};

point_light *add_point_light() {
  point_lights = realloc(point_lights, sizeof(point_light *) * (point_light_count + 1));
  point_light *l = malloc(sizeof(point_light));
  l->x = 0;
  l->y = 0;
  l->z = 0;
  l->brightness = 10;
  l->attenuation_mode = GRAFFIKS_LIGHT_ATTENUATION_LINEAR;
  l->attenuation_strength = 1;

  point_lights[point_light_count++] = l;

  return l;
}

void set_ambient_light(float r, float g, float b) {
  ambient_color[0] = r;
  ambient_color[1] = g;
  ambient_color[2] = b;
}

void remove_point_light(point_light *light) {
  // shift all elements to the right of the light once to the left
  bool past_light = false;
  int i;
  for (i = 0; i < point_light_count; i++) {
    if (past_light) {
      point_lights[i - 1] = point_lights[i];
    }

    if (point_lights[i] == light) {
      past_light = true;
    }
  }

  point_light_count--;

  // realloc the array to the new (smaller) size
  point_lights = realloc(point_lights, sizeof(point_light *) * (point_light_count));

  // if this was the last light, free the array
  if (point_light_count == 0) {
    free(point_lights);
  }

  // free the light
  free(light);
}
