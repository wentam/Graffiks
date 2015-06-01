#ifndef MESH_H
#define MESH_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#ifdef ANDROID
#include <GLES2/gl2.h>
#endif

#ifndef DLL_EXPORT
# ifdef _WIN32
#  ifdef GRAFFIKS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# else
#  define DLL_EXPORT
# endif
#endif

typedef enum { false, true } bool;

typedef struct {
  GLuint triangle_buffer;
  GLuint normal_buffer;
  GLuint vertex_color_buffer;
  int vertex_count;
  int normal_count;
  float location_x;
  float location_y;
  float location_z;
  float angle;
  float rot_x;
  float rot_y;
  float rot_z;
  bool use_vertex_color; // because perl6 doesn't support native enums yet,
                         // keep this bool as the last element.
                         // this is for perl6 bindings to the engine.
} mesh;

typedef struct {
  int vertex_count;
  int normal_count;
  float *vertices;
  float *normals;
} base_mesh;

DLL_EXPORT mesh *create_mesh(float **vertices, int ***indicies, int index_count, float **normals);
DLL_EXPORT mesh *create_mesh_st(float vertices[][3], int indicies[][3][3], int index_count,
                     float normals[][3]);
DLL_EXPORT base_mesh *create_base_mesh(float **vertices, int ***indicies, int index_count,
                            float **normals);
DLL_EXPORT mesh *create_mesh_with_instances(base_mesh *mesh, double instances[][3],
                                 int instance_count, bool use_z);
DLL_EXPORT void set_mesh_vertex_colors(mesh *m, float colors[][4], int ***indicies, int index_count);
DLL_EXPORT void free_mesh(mesh *mesh);
DLL_EXPORT void free_base_mesh(base_mesh *mesh);

// internal stuff
DLL_EXPORT mesh *_allocate_mesh(int index_count);
DLL_EXPORT void _generate_mesh(float output_vertices[], float output_normals[], float **vertices,
                    int ***indicies, int index_count, float **normals);
DLL_EXPORT void _generate_mesh_st(float output_vertices[], float output_normals[],
                       float vertices[][3], int indicies[][3][3], int index_count,
                       float normals[][3]);
#endif
