#ifndef LIGHTS_H
#define LIGHTS_H
#include <stdbool.h>

extern float ambient_color[4]; // rgbi (i == intensity)

extern const int GRAFFIKS_LIGHT_ATTENUATION_LINEAR;
extern const int GRAFFIKS_LIGHT_ATTENUATION_QUADRATIC;
extern const int GRAFFIKS_LIGHT_ATTENUATION_CONSTANT;

typedef struct {
  float x;
  float y;
  float z;
  float brightness;           // TODO unimplemented
  int attenuation_mode;       // TODO unimplemented
  float attenuation_strength; // TODO unimplemented
  float color_r;              // TODO unimplemented
  float color_g;              // TODO unimplemented
  float color_b;              // TODO unimplemented
} point_light;

point_light **point_lights;
extern int point_light_count;

point_light *add_point_light();
void remove_point_light(point_light *light);
void set_ambient_light(float r, float g, float b, float intensity);

#endif
