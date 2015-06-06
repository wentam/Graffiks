/// \file
/// \brief Provides functions for creating and managing cameras

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

/// \cond INTERNAL
float gfks_view_matrix[16];
extern gfks_camera *_gfks_active_camera;
/// \endcond

/// \brief Creates a new camera
/// \return The new camera
DLL_EXPORT gfks_camera *gfks_create_camera();

/// \brief Destroys a camera, freeing it from memory
/// \param c The camera to destroy
DLL_EXPORT void gfks_destroy_camera(gfks_camera *c);

/// \brief Sets a camera as the active camera
/// \param c The camera to make active
DLL_EXPORT void gfks_set_active_camera(gfks_camera *c);

/// \brief Sets the location of a camera
/// \param c The camera
/// \param x New location on the x axis
/// \param y New location on the y axis
/// \param z New location on the z axis
DLL_EXPORT void gfks_set_camera_location(gfks_camera *c, float x, float y, float z);

/// \brief Sets the point that the camera will rotate to aim at
///
/// If you set this to 0,0,0, then the camera will rotate to always point to 0,0,0 even as
/// you translate it
/// \param c The camera
/// \param x The target location on the x axis
/// \param y The target location on the y axis
/// \param z The target location on the z axis
DLL_EXPORT void gfks_set_camera_target(gfks_camera *c, float x, float y, float z);

/// \brief Rotates the camera
///
/// Uses quaternion rotation (xyzw).
/// \param c The camera to rotate
/// \param x The x component of quaternion rotation
/// \param y The y component of quaternion rotation
/// \param z The z component of quaternion rotation
/// \param w The w component of quaternion rotation
DLL_EXPORT void gfks_rotate_camera(gfks_camera *c, float x, float y, float z, float w);

/// \brief Returns the active camera
/// \return A pointer to the active camera
DLL_EXPORT gfks_camera *gfks_get_active_camera();

/// \brief Returns the camera location on the x axis
/// \return The camera location on the x axis
DLL_EXPORT float gfks_get_camera_location_x(gfks_camera *c);

/// \brief Returns the camera location on the y axis
/// \return The camera location on the y axis
DLL_EXPORT float gfks_get_camera_location_y(gfks_camera *c);

/// \brief Returns the camera location on the z axis
/// \return The camera location on the z axis
DLL_EXPORT float gfks_get_camera_location_z(gfks_camera *c);

/// \brief Returns the camera target location on the x axis
/// \return The camera target location on the x axis
DLL_EXPORT float gfks_get_camera_target_x(gfks_camera *c);

/// \brief Returns the camera target location on the y axis
/// \return The camera target location on the y axis
DLL_EXPORT float gfks_get_camera_target_y(gfks_camera *c);

/// \brief Returns the camera target location on the z axis
/// \return The camera target location on the z axis
DLL_EXPORT float gfks_get_camera_target_z(gfks_camera *c);

/// \cond INTERNAL
DLL_EXPORT void _gfks_update_view_matrix();
/// \endcond
#endif
