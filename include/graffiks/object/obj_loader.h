/// \file

#ifndef GFKS_OBJ_LOADER_H
#define GFKS_OBJ_LOADER_H

#include "graffiks/mesh/mesh.h"
#include <stdio.h>
#include <string.h>
#include "graffiks/object/object.h"
#include "graffiks/material.h"
#include "graffiks/renderer/renderer.h"

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

DLL_EXPORT gfks_object *gfks_load_obj(gfks_renderer_flags flags, char *filepath);

#endif
