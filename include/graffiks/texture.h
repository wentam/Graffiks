/// \file

#ifndef GFKS_TEXTURE_H
#define GFKS_TEXTURE_H

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
  float **uv;
  GLuint tex;
} gfks_texture;

DLL_EXPORT gfks_texture *gfks_create_texture(float **uv, char *filepath);
DLL_EXPORT void gfks_free_texture(gfks_texture *t);

#endif
