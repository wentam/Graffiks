#include "renderer/forward_renderer.h"
#include "governor.h"
#include "material/material.h"
#include "material/material_fw.h"
#include "lights.h"
#include "renderer/renderer.h"

GLuint *program;

void _terminate_renderer_fw() {}
void _init_renderer_fw() {
  program = _create_program("/shaders/material.vert", "/shaders/material.frag");
}

void _draw_mesh_point_light(object *o, mesh *m, material *mat, point_light *l) {

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

  create_identity_matrix(object_model_matrix);
  create_identity_matrix(mesh_model_matrix);

  translate_matrix(object_model_matrix, o->location_x, o->location_y, o->location_z);
  translate_matrix(mesh_model_matrix, m->location_x, m->location_y, m->location_z);

  set_matrix_rotation(object_rotation_matrix, o->angle, o->rot_x, o->rot_y, o->rot_z);
  set_matrix_rotation(mesh_rotation_matrix, m->angle, m->rot_x, m->rot_y, m->rot_z);

  multiply_matrices(local_model_rotation_matrix, mesh_model_matrix, mesh_rotation_matrix);
  multiply_matrices(model_rotation_matrix, object_model_matrix, object_rotation_matrix);
  multiply_matrices(combined_model_rotation_matrix, model_rotation_matrix,
                    local_model_rotation_matrix);

  multiply_matrices(model_view_matrix, view_matrix, combined_model_rotation_matrix);
  multiply_matrices(model_view_projection_matrix, projection_matrix, model_view_matrix);

  // send our data to the shader program
  glUniformMatrix4fv(GRAFFIKS_MATERIAL_FW_UATTRIB_MVP_MATRIX, 1, GL_FALSE,
                     model_view_projection_matrix);
  glUniformMatrix4fv(GRAFFIKS_MATERIAL_FW_UATTRIB_MV_MATRIX, 1, GL_FALSE,
                     model_view_matrix);
  glUniform4f(GRAFFIKS_MATERIAL_FW_UATTRIB_AMBIENT_COLOR, ambient_color[0],
              ambient_color[1], ambient_color[2], ambient_color[3]);
  glUniform4f(GRAFFIKS_MATERIAL_FW_UATTRIB_DIFFUSE_COLOR, mat->diffuse_color[0],
              mat->diffuse_color[1], mat->diffuse_color[2], mat->diffuse_color[3]);
  glUniform3f(GRAFFIKS_MATERIAL_FW_UATTRIB_LIGHT_POSITION, l->x, l->y, l->z);
  glUniform1f(GRAFFIKS_MATERIAL_FW_UATTRIB_DIFFUSE_INTENSITY, mat->diffuse_intensity);

  // add vertices
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(GRAFFIKS_MATERIAL_FW_ATTRIB_POSITION);
  glVertexAttribPointer(GRAFFIKS_MATERIAL_FW_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0,
                        0);

  // add normals
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glEnableVertexAttribArray(GRAFFIKS_MATERIAL_FW_ATTRIB_NORMAL);
  glVertexAttribPointer(GRAFFIKS_MATERIAL_FW_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add colors if m->use_vertex_color
  if (m->use_vertex_color) {
    glUniform1i(GRAFFIKS_MATERIAL_FW_UATTRIB_PER_VERTEX, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
    glEnableVertexAttribArray(GRAFFIKS_MATERIAL_FW_ATTRIB_DIFFUSE_COLOR);
    glVertexAttribPointer(GRAFFIKS_MATERIAL_FW_ATTRIB_DIFFUSE_COLOR, 4, GL_FLOAT,
                          GL_FALSE, 0, 0);
  } else {
    glUniform1i(GRAFFIKS_MATERIAL_FW_UATTRIB_PER_VERTEX, 0);
  }

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  // disable arrays
  glDisableVertexAttribArray(GRAFFIKS_MATERIAL_FW_ATTRIB_POSITION);
  glDisableVertexAttribArray(GRAFFIKS_MATERIAL_FW_ATTRIB_NORMAL);
}

void _draw_from_queue_fw() {
  glUseProgram(*program);

  // TODO depth buffer pre-pass
  int i;
  for (i = 0; i < render_queue_size; i++) {

    if (render_queue[i]->material->renderer & GRAFFIKS_RENDERER_FORWARD) {
      int i2;
      for (i2 = 0; i2 < point_light_count; i2++) {
        if (i2 != 0) {
          glEnable(GL_BLEND);
          glBlendEquation(GL_FUNC_ADD);
          glBlendFunc(GL_ONE, GL_ONE);
        } else {
          glDisable(GL_BLEND);
        }

        _draw_mesh_point_light(render_queue[i]->parent_object, render_queue[i]->mesh,
                               render_queue[i]->material, point_lights[i2]);
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
void _draw_object_fw(object *o) {
  int i2;
  for (i2 = 0; i2 < point_light_count; i2++) {
    if (i2 == 0) {
    } else {
      glEnable(GL_BLEND);
      glBlendEquation(GL_FUNC_ADD);
      glBlendFunc(GL_ONE, GL_ONE);
    }

    int i;
    for (i = 0; i < o->mesh_count; i++) {
      _draw_mesh_point_light(o, o->meshes[i], o->mats[i], point_lights[i2]);
    }
  }
  glDisable(GL_BLEND);
}

void _clear_fw() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
