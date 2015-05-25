#ifndef LIGHTS_H
#define LIGHTS_H
#include <stdbool.h>

extern const int GRAFFIKS_LIGHT_ATTENUATION_LINEAR;
extern const int GRAFFIKS_LIGHT_ATTENUATION_QUADRATIC;
extern const int GRAFFIKS_LIGHT_ATTENUATION_CONSTANT;

typedef struct {
  float x;
  float y;
  float z;
  float brightness;
  int attenuation_mode;
  float attenuation_strength;
} point_light;

point_light **point_lights;
extern int point_light_count;

point_light *add_point_light();
void remove_point_light(point_light *light);

#endif
