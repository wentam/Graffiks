/// \file

#ifndef GFKS_PRIMITIVES_H
#define GFKS_PRIMITIVES_H

#include "graffiks/mesh.h"

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

DLL_EXPORT gfks_mesh *gfks_create_cube(float scale);
DLL_EXPORT gfks_mesh *gfks_create_plane(float width, float height);
DLL_EXPORT gfks_mesh *gfks_create_triangle(float scale);

#endif
