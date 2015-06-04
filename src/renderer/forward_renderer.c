#include <GL/glew.h>
#include "graffiks/renderer/forward_renderer.h"
#include "graffiks/governor.h"
#include "graffiks/material.h"
#include "graffiks/lights.h"
#include "graffiks/renderer/renderer.h"

GLuint *fw_point_light_program;
GLuint *fw_ambient_light_program;

void _gfks_terminate_renderer_fw() {}
void _gfks_init_renderer_fw() {
  fw_point_light_program = _gfks_create_program("/shaders/forward/point_light.vert",
                                                "/shaders/forward/point_light.frag");
  fw_ambient_light_program = _gfks_create_program("/shaders/forward/ambient_light.vert",
                                                  "/shaders/forward/ambient_light.frag");
}

const GLint FW_POINT_LIGHT_ATTRIB_POSITION = 0;
const GLint FW_POINT_LIGHT_ATTRIB_NORMAL = 1;
const GLint FW_POINT_LIGHT_ATTRIB_DIFFUSE_COLOR = 2;
const GLint FW_POINT_LIGHT_UATTRIB_MVP_MATRIX = 3;
const GLint FW_POINT_LIGHT_UATTRIB_MV_MATRIX = 4;
const GLint FW_POINT_LIGHT_UATTRIB_DIFFUSE_COLOR = 6;
const GLint FW_POINT_LIGHT_UATTRIB_PER_VERTEX = 8;
const GLint FW_POINT_LIGHT_UATTRIB_SPEC_HARDNESS = 10;
const GLint FW_POINT_LIGHT_UATTRIB_SPEC_COLOR = 11;
const GLint FW_POINT_LIGHT_UATTRIB_LIGHT_POSITION = 100;
const GLint FW_POINT_LIGHT_UATTRIB_LIGHT_BRIGHTNESS = 101;

void _gfks_draw_mesh_point_light(gfks_object *o, gfks_mesh *m, gfks_material *mat,
                                 gfks_point_light *l) {

  // set up matrices
  float object_rotation_matrix[16];
  float mesh_rotation_matrix[16];
  float object_model_matrix[16];
  float mesh_model_matrix[16];

  float local_model_rotation_matrix[16];
  float model_rotation_matrix[16];
  float combined_model_rotation_matrix[16];

  float model_view_matrix[16];
  float model_view_projection_matrix[16];

  gfks_create_identity_matrix(object_model_matrix);
  gfks_create_identity_matrix(mesh_model_matrix);

  gfks_translate_matrix(object_model_matrix, o->location_x, o->location_y, o->location_z);
  gfks_translate_matrix(mesh_model_matrix, m->location_x, m->location_y, m->location_z);

  gfks_set_matrix_rotation(object_rotation_matrix, o->angle, o->rot_x, o->rot_y,
                           o->rot_z);
  gfks_set_matrix_rotation(mesh_rotation_matrix, m->angle, m->rot_x, m->rot_y, m->rot_z);

  gfks_multiply_matrices(local_model_rotation_matrix, mesh_model_matrix,
                         mesh_rotation_matrix);
  gfks_multiply_matrices(model_rotation_matrix, object_model_matrix,
                         object_rotation_matrix);
  gfks_multiply_matrices(combined_model_rotation_matrix, model_rotation_matrix,
                         local_model_rotation_matrix);

  gfks_multiply_matrices(model_view_matrix, gfks_view_matrix,
                         combined_model_rotation_matrix);
  gfks_multiply_matrices(model_view_projection_matrix, gfks_projection_matrix,
                         model_view_matrix);

  // send our data to the shader program
  glUniformMatrix4fv(FW_POINT_LIGHT_UATTRIB_MVP_MATRIX, 1, GL_FALSE,
                     model_view_projection_matrix);
  glUniformMatrix4fv(FW_POINT_LIGHT_UATTRIB_MV_MATRIX, 1, GL_FALSE, model_view_matrix);
  glUniform4f(FW_POINT_LIGHT_UATTRIB_DIFFUSE_COLOR, mat->diffuse_color[0],
              mat->diffuse_color[1], mat->diffuse_color[2], mat->diffuse_color[3]);
  glUniform3f(FW_POINT_LIGHT_UATTRIB_LIGHT_POSITION, l->x, l->y, l->z);
  glUniform1f(FW_POINT_LIGHT_UATTRIB_LIGHT_BRIGHTNESS, l->brightness);
  glUniform1f(FW_POINT_LIGHT_UATTRIB_SPEC_HARDNESS, mat->specularity_hardness);
  glUniform3f(FW_POINT_LIGHT_UATTRIB_SPEC_COLOR, mat->specularity_color_r,
              mat->specularity_color_g, mat->specularity_color_b);

  // add vertices
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(FW_POINT_LIGHT_ATTRIB_POSITION);
  glVertexAttribPointer(FW_POINT_LIGHT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add normals
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glEnableVertexAttribArray(FW_POINT_LIGHT_ATTRIB_NORMAL);
  glVertexAttribPointer(FW_POINT_LIGHT_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add colors if m->use_vertex_color
  if (m->use_vertex_color) {
    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
    glEnableVertexAttribArray(FW_POINT_LIGHT_ATTRIB_DIFFUSE_COLOR);
    glVertexAttribPointer(FW_POINT_LIGHT_ATTRIB_DIFFUSE_COLOR, 4, GL_FLOAT, GL_FALSE, 0,
                          0);
  } else {
    glUniform1i(FW_POINT_LIGHT_UATTRIB_PER_VERTEX, 0);
  }

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  // disable arrays
  glDisableVertexAttribArray(FW_POINT_LIGHT_ATTRIB_POSITION);
  glDisableVertexAttribArray(FW_POINT_LIGHT_ATTRIB_NORMAL);
}

const GLint FW_AMBIENT_LIGHT_ATTRIB_POSITION = 0;
const GLint FW_AMBIENT_LIGHT_UATTRIB_MVP_MATRIX = 1;
const GLint FW_AMBIENT_LIGHT_UATTRIB_MV_MATRIX = 2;
const GLint FW_AMBIENT_LIGHT_UATTRIB_AMBIENT_COLOR = 3;

void _gfks_draw_mesh_ambient_light(gfks_object *o, gfks_mesh *m) {

  // set up matrices
  float object_rotation_matrix[16];
  float mesh_rotation_matrix[16];
  float object_model_matrix[16];
  float mesh_model_matrix[16];

  float local_model_rotation_matrix[16];
  float model_rotation_matrix[16];
  float combined_model_rotation_matrix[16];

  float model_view_matrix[16];
  float model_view_projection_matrix[16];

  gfks_create_identity_matrix(object_model_matrix);
  gfks_create_identity_matrix(mesh_model_matrix);

  gfks_translate_matrix(object_model_matrix, o->location_x, o->location_y, o->location_z);
  gfks_translate_matrix(mesh_model_matrix, m->location_x, m->location_y, m->location_z);

  gfks_set_matrix_rotation(object_rotation_matrix, o->angle, o->rot_x, o->rot_y,
                           o->rot_z);
  gfks_set_matrix_rotation(mesh_rotation_matrix, m->angle, m->rot_x, m->rot_y, m->rot_z);

  gfks_multiply_matrices(local_model_rotation_matrix, mesh_model_matrix,
                         mesh_rotation_matrix);
  gfks_multiply_matrices(model_rotation_matrix, object_model_matrix,
                         object_rotation_matrix);
  gfks_multiply_matrices(combined_model_rotation_matrix, model_rotation_matrix,
                         local_model_rotation_matrix);

  gfks_multiply_matrices(model_view_matrix, gfks_view_matrix,
                         combined_model_rotation_matrix);
  gfks_multiply_matrices(model_view_projection_matrix, gfks_projection_matrix,
                         model_view_matrix);

  glUniformMatrix4fv(FW_AMBIENT_LIGHT_UATTRIB_MVP_MATRIX, 1, GL_FALSE,
                     model_view_projection_matrix);
  glUniformMatrix4fv(FW_AMBIENT_LIGHT_UATTRIB_MV_MATRIX, 1, GL_FALSE, model_view_matrix);
  glUniform3f(FW_AMBIENT_LIGHT_UATTRIB_AMBIENT_COLOR, gfks_ambient_color[0],
              gfks_ambient_color[1], gfks_ambient_color[2]);

  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(FW_AMBIENT_LIGHT_ATTRIB_POSITION);
  glVertexAttribPointer(FW_AMBIENT_LIGHT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  glDisableVertexAttribArray(FW_AMBIENT_LIGHT_ATTRIB_POSITION);
}

void _gfks_draw_from_queue_fw() {

  // TODO depth buffer pre-pass
  int i;
  for (i = 0; i < gfks_render_queue_size; i++) {

    if (gfks_render_queue[i]->material->renderer & GFKS_RENDERER_FORWARD) {
      glUseProgram(*fw_ambient_light_program);
      glDisable(GL_BLEND);
      _gfks_draw_mesh_ambient_light(gfks_render_queue[i]->parent_object,
                                    gfks_render_queue[i]->mesh);

      glUseProgram(*fw_point_light_program);
      int i2;
      for (i2 = 0; i2 < gfks_point_light_count; i2++) {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        _gfks_draw_mesh_point_light(
            gfks_render_queue[i]->parent_object, gfks_render_queue[i]->mesh,
            gfks_render_queue[i]->material, gfks_point_lights[i2]);
      }
    }
  }
  glDisable(GL_BLEND);
}

//
// void _draw_object_fw(object *o) {
//     int i;
//     for (i = 0; i < o->mesh_count; i++) {
//     _draw_mesh_point_light(o, o->meshes[i], o->mats[i], point_lights[i2]);
//     }
// }
void _gfks_draw_object_fw(gfks_object *o) {
  int i2;
  for (i2 = 0; i2 < gfks_point_light_count; i2++) {
    if (i2 == 0) {
    } else {
      glEnable(GL_BLEND);
      glBlendEquation(GL_FUNC_ADD);
      glBlendFunc(GL_ONE, GL_ONE);
    }

    int i;
    for (i = 0; i < o->mesh_count; i++) {
      _gfks_draw_mesh_point_light(o, o->meshes[i], o->mats[i], gfks_point_lights[i2]);
    }
  }
  glDisable(GL_BLEND);
}

void _gfks_clear_fw() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
