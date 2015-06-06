/// \file

#ifndef GFKS_CAMERA_H
#define GFKS_CAMERA_H

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

float gfks_view_matrix[16];

typedef struct {
  float location_x;
  float location_y;
  float location_z;
  float target_x;
  float target_y;
  float target_z;
  float up_x;
  float up_y;
  float up_z;
} gfks_camera;

extern gfks_camera *_gfks_active_camera;

DLL_EXPORT gfks_camera *gfks_create_camera();
DLL_EXPORT void gfks_destroy_camera(gfks_camera *c);

DLL_EXPORT void gfks_set_active_camera(gfks_camera *c);

DLL_EXPORT void gfks_set_camera_location(gfks_camera *c, float x, float y, float z);
DLL_EXPORT void gfks_set_camera_target(gfks_camera *c, float x, float y, float z);
DLL_EXPORT void gfks_rotate_camera(gfks_camera *c, float x, float y, float z, float w);

DLL_EXPORT gfks_camera *gfks_get_active_camera();

DLL_EXPORT float gfks_get_camera_location_x(gfks_camera *c);
DLL_EXPORT float gfks_get_camera_location_y(gfks_camera *c);
DLL_EXPORT float gfks_get_camera_location_z(gfks_camera *c);

DLL_EXPORT float gfks_get_camera_target_x(gfks_camera *c);
DLL_EXPORT float gfks_get_camera_target_y(gfks_camera *c);
DLL_EXPORT float gfks_get_camera_target_z(gfks_camera *c);

DLL_EXPORT void _gfks_update_view_matrix();
#endif
