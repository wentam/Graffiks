/// \file lights.h
#ifndef GFKS_LIGHTS_H
#define GFKS_LIGHTS_H
#include <stdbool.h>

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

/// \brief The color of the ambient light
extern float gfks_ambient_color[3]; // rgb

extern const int GFKS_LIGHT_ATTENUATION_LINEAR;
extern const int GFKS_LIGHT_ATTENUATION_QUADRATIC;
extern const int GFKS_LIGHT_ATTENUATION_CONSTANT;

typedef struct {
  float x;
  float y;
  float z;
  float brightness;
  int attenuation_mode;       // TODO unimplemented
  float attenuation_strength; // TODO unimplemented
  float color_r;              // TODO unimplemented
  float color_g;              // TODO unimplemented
  float color_b;              // TODO unimplemented
} gfks_point_light;

/// \brief Returns the position of the light on the x axis
DLL_EXPORT float gfks_get_point_light_x(gfks_point_light *l);

/// \brief Returns the position of the light on the y axis
DLL_EXPORT float gfks_get_point_light_y(gfks_point_light *l);

/// \brief Returns the position of the light on the z axis
DLL_EXPORT float gfks_get_point_light_z(gfks_point_light *l);

/// \brief Returns the brightness of the light
DLL_EXPORT float gfks_get_point_light_brightness(gfks_point_light *l);

/// \brief Sets the position of a light
DLL_EXPORT void gfks_set_point_light_location(gfks_point_light *l, float x, float y,
                                              float z);

/// \brief Sets the brightness of a light
DLL_EXPORT void gfks_set_point_light_brightness(gfks_point_light *l, float brightness);

gfks_point_light **gfks_point_lights;
extern int gfks_point_light_count;

DLL_EXPORT gfks_point_light *gfks_add_point_light();
DLL_EXPORT void gfks_remove_point_light(gfks_point_light *light);
DLL_EXPORT void gfks_set_ambient_light(float r, float g, float b);

#endif
