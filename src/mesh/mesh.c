#include <GL/glew.h>
#include "graffiks/mesh/mesh.h"
#include <stdlib.h>
#include <stdio.h>

mesh *create_mesh(float **vertices, int ***indicies, int index_count, float **normals) {
  float *system_vertices = malloc(sizeof(float) * ((index_count * 3) * 3));
  float *system_normals = malloc(sizeof(float) * ((index_count * 3) * 3));
  _generate_mesh(system_vertices, system_normals, vertices, indicies, index_count,
                 normals);

  mesh *m = _allocate_mesh(index_count);

  glGenBuffers(1, &m->triangle_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((index_count * 3) * 3), system_vertices,
               GL_STATIC_DRAW);

  glGenBuffers(1, &m->normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ((index_count * 3) * 3), system_normals,
               GL_STATIC_DRAW);

  free(system_vertices);
  free(system_normals);
  return m;
}

// colors:
// [
// [0..1,0..1,0..1,0..1] ([r,g,b,a])
// ...
// ]
// colors MUST contain the same number of elements as there are vertices
void set_mesh_vertex_colors(mesh *m, float colors[][4], int ***indicies,
                            int index_count) {
  // enable vertex coloring in mesh
  m->use_vertex_color = true;

  // flatten the colors 2d array
  float colors_flat[(index_count * 3) * 4];

  int current_vertex_index = 0;
  int i;
  int i2;
  for (i = 0; i < index_count; i++) {
    for (i2 = 0; i2 < 3; i2++) {
      colors_flat[current_vertex_index] = colors[indicies[i][i2][0]][0];
      current_vertex_index++;
      colors_flat[current_vertex_index] = colors[indicies[i][i2][0]][1];
      current_vertex_index++;
      colors_flat[current_vertex_index] = colors[indicies[i][i2][0]][2];
      current_vertex_index++;
      colors_flat[current_vertex_index] = colors[indicies[i][i2][0]][3];
      current_vertex_index++;
    }
  }

  // create buffer
  glGenBuffers(1, &m->vertex_color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_flat), colors_flat, GL_STATIC_DRAW);
}

mesh *create_mesh_st(float vertices[][3], int indicies[][3][3], int index_count,
                     float normals[][3]) {
  float system_vertices[(index_count * 3) * 3];
  float system_normals[(index_count * 3) * 3];
  _generate_mesh_st(system_vertices, system_normals, vertices, indicies, index_count,
                    normals);

  mesh *m = _allocate_mesh(index_count);

  glGenBuffers(1, &m->triangle_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(system_vertices), system_vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &m->normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(system_normals), system_normals, GL_STATIC_DRAW);
  return m;
}

base_mesh *create_base_mesh(float **vertices, int ***indicies, int index_count,
                            float **normals) {
  float *system_vertices = malloc(sizeof(float) * (index_count * 3 * 3));
  float *system_normals = malloc(sizeof(float) * (index_count * 3 * 3));

  _generate_mesh(system_vertices, system_normals, vertices, indicies, index_count,
                 normals);

  base_mesh *bmesh = malloc(sizeof(base_mesh));
  bmesh->vertex_count = index_count * 3 * 3;
  bmesh->normal_count = index_count * 3 * 3;
  bmesh->vertices = system_vertices;
  bmesh->normals = system_normals;

  return bmesh;
}

mesh *create_mesh_with_instances(base_mesh *bmesh, double instances[][3],
                                 int instance_count, bool use_z) {
  float system_vertices[bmesh->vertex_count * instance_count];
  float system_normals[bmesh->normal_count * instance_count];

  int start_index = 0;
  int start_index_normal = 0;
  int index_offset = 0;
  int i;
  int i2;
  for (i = 0; i < instance_count; i++) {
    // vertices
    for (i2 = 0; i2 < bmesh->vertex_count; i2++) {
      if (index_offset == 2 && use_z == false) {
        system_vertices[start_index + i2] = bmesh->vertices[i2];
      } else {
        system_vertices[start_index + i2] =
            bmesh->vertices[i2] + instances[i][index_offset];
      }

      index_offset++;
      if (index_offset >= 3) {
        index_offset = 0;
      }
    }

    start_index += bmesh->vertex_count;

    // normals
    for (i2 = 0; i2 < bmesh->normal_count; i2++) {
      system_normals[start_index_normal + i2] = bmesh->normals[i2];
    }

    start_index_normal += bmesh->normal_count;
  }

  mesh *smesh = malloc(sizeof(mesh));
  smesh->vertex_count = bmesh->vertex_count * instance_count;
  smesh->normal_count = bmesh->normal_count * instance_count;
  smesh->location_x = 0;
  smesh->location_y = 0;
  smesh->location_z = 0;
  smesh->angle = 0;
  smesh->rot_x = 1;
  smesh->rot_y = 0;
  smesh->rot_z = 0;
  /*smesh->triangle_buffer = malloc(sizeof(GLuint));*/
  /*smesh->normal_buffer = malloc(sizeof(GLuint));*/

  // make buffers
  /*glGenBuffers(1, smesh->triangle_buffer);*/
  /*glBindBuffer(GL_ARRAY_BUFFER, *(smesh->triangle_buffer));*/
  /*glBufferData(GL_ARRAY_BUFFER,
   * sizeof(float)*(bmesh->vertex_count*instance_count), system_vertices,
   * GL_DYNAMIC_DRAW);*/

  /*glGenBuffers(1, smesh->normal_buffer);*/
  /*glBindBuffer(GL_ARRAY_BUFFER, *(smesh->normal_buffer));*/
  /*glBufferData(GL_ARRAY_BUFFER,
   * sizeof(float)*(bmesh->normal_count*instance_count), system_normals,
   * GL_DYNAMIC_DRAW);*/

  return smesh;
}

void free_mesh(mesh *m) {
  glDeleteBuffers(1, &m->triangle_buffer);
  glDeleteBuffers(1, &m->normal_buffer);
  if (m->use_vertex_color) {
    glDeleteBuffers(1, &m->vertex_color_buffer);
  }
  free(m);
}

void free_base_mesh(base_mesh *m) {
  free(m->vertices);
  free(m->normals);
  free(m);
}

mesh *_allocate_mesh(int index_count) {

  mesh *m = malloc(sizeof(mesh));
  m->use_vertex_color = false;
  m->vertex_count = (index_count * 3) * 3;
  m->normal_count = (index_count * 3) * 3;
  m->location_x = 0;
  m->location_y = 0;
  m->location_z = 0;
  m->angle = 0;
  m->rot_x = 0;
  m->rot_y = 0;
  m->rot_z = 1;

  return m;
}

void _generate_mesh(float output_vertices[], float output_normals[], float **vertices,
                    int ***indicies, int index_count, float **normals) {

  int current_vertex_index = 0;
  int current_normal_index = 0;
  int i;
  int i2;
  for (i = 0; i < index_count; i++) {
    // we need to create 3 vertices and 3 normals per index in indicies
    for (i2 = 0; i2 < 3; i2++) {
      // vertex
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][0];
      current_vertex_index++;
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][1];
      current_vertex_index++;
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][2];
      current_vertex_index++;

      // normal
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][0];
      current_normal_index++;
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][1];
      current_normal_index++;
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][2];
      current_normal_index++;
    }
  }
}

void _generate_mesh_st(float output_vertices[], float output_normals[],
                       float vertices[][3], int indicies[][3][3], int index_count,
                       float normals[][3]) {

  int current_vertex_index = 0;
  int current_normal_index = 0;
  int i;
  int i2;
  for (i = 0; i < index_count; i++) {
    // we need to create 3 vertices and 3 normals per index in indicies
    for (i2 = 0; i2 < 3; i2++) {
      // vertex
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][0];
      current_vertex_index++;
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][1];
      current_vertex_index++;
      output_vertices[current_vertex_index] = vertices[indicies[i][i2][0]][2];
      current_vertex_index++;

      // normal
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][0];
      current_normal_index++;
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][1];
      current_normal_index++;
      output_normals[current_normal_index] = normals[indicies[i][i2][2]][2];
      current_normal_index++;
    }
  }
}

void _dm(float m[]) {
  int i;
#ifdef ANDROID
  __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "--");
#endif
  for (i = 0; i < 4; i++) {
    int i2;
    float row[4];
    for (i2 = 0; i2 < 4; i2++) {
      row[i2] = m[(i * 4) + i2];
    }
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "[%f, %f, %f, %f]", row[0], row[1],
                        row[2], row[3]);
#endif
  }
#ifdef ANDROID
  __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "--");
#endif
}
