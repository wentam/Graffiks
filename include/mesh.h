#ifndef MESH_H
#define MESH_H
#include <GLES2/gl2.h>
#include <stdlib.h>
#include "material.h"
#include "renderer.h"

typedef struct {
    GLuint triangle_buffer;
    GLuint normal_buffer;
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

typedef enum { false, true } bool;

mesh* create_mesh(float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]);
base_mesh* create_base_mesh(float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]);
mesh* create_mesh_with_instances(base_mesh *mesh, double instances[][3], int instance_count, bool use_z);
void free_mesh(mesh *mesh);
void free_base_mesh(base_mesh *mesh);
void draw_mesh(mesh *m, material *mat);

// internal stuff
void _generate_mesh(float output_vertices[], float output_normals[],
        float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]);
#endif
