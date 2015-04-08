#ifndef MESH_H
#define MESH_H
#include <GLES2/gl2.h>
#include <stdlib.h>
#include "material.h"
#include "renderer.h"

typedef enum { false, true } bool;

typedef struct {
    GLuint triangle_buffer;
    GLuint normal_buffer;
    GLuint vertex_color_buffer;
    bool use_vertex_color;
    int vertex_count;
    int normal_count;
    float location_x;
    float location_y;
    float location_z;
    float angle;
    float rot_x;
    float rot_y;
    float rot_z;
} mesh;

typedef struct {
    int vertex_count;
    int normal_count;
    float *vertices;
    float *normals;
} base_mesh;


mesh* create_mesh(float **vertices, int ***indicies, int index_count, float **normals);
mesh* create_mesh_st(float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]);
base_mesh* create_base_mesh(float **vertices, int ***indicies, int index_count, float **normals);
mesh* create_mesh_with_instances(base_mesh *mesh, double instances[][3], int instance_count, bool use_z);
void set_mesh_vertex_colors(mesh *m, float colors[][4], int ***indicies, int index_count);
void free_mesh(mesh *mesh);
void free_base_mesh(base_mesh *mesh);
void draw_mesh(mesh *m, material *mat);

// internal stuff
mesh* _allocate_mesh(int index_count);
void _generate_mesh(float output_vertices[], float output_normals[],
        float **vertices, int ***indicies, int index_count, float **normals);
void _generate_mesh_st(float output_vertices[], float output_normals[],
        float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]);
#endif
