#include "renderer/forward_renderer.h"

void _destroy_renderer_fw() {}
void _init_renderer_fw() {}

void _draw_mesh(object *o, mesh *m, material *mat) {
  GLuint *program = mat->program;
  glUseProgram(*program);

  // get GLSL variable locations
  GLint u_mvp_matrix_location =
      glGetUniformLocation(*program, "u_mvp_matrix"); // model*view*projection
  GLint u_mv_matrix_location =
      glGetUniformLocation(*program, "u_mv_matrix"); // model*view
  GLint u_ambient_color_location = glGetUniformLocation(*program, "u_ambient_color");
  GLint u_diffuse_color_location = glGetUniformLocation(*program, "u_diffuse_color");
  GLint u_diffuse_intensity_location =
      glGetUniformLocation(*program, "u_diffuse_intensity");
  GLint u_light_position_location = glGetUniformLocation(*program, "u_light_position");
  GLint u_per_vertex_location = glGetUniformLocation(*program, "u_per_vertex");
  GLint a_position_location = glGetAttribLocation(*program, "a_position");
  GLint a_normal_location = glGetAttribLocation(*program, "a_normal");
  GLint a_diffuse_color_location = glGetAttribLocation(*program, "a_diffuse_color");

  // add vertices
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(a_position_location);
  glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add normals
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glEnableVertexAttribArray(a_normal_location);
  glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add colors if m->use_vertex_color
  if (m->use_vertex_color) {
    glUniform1i(u_per_vertex_location, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
    glEnableVertexAttribArray(a_diffuse_color_location);
    glVertexAttribPointer(a_diffuse_color_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    glUniform1i(u_per_vertex_location, 0);
  }

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
  glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, model_view_projection_matrix);
  glUniformMatrix4fv(u_mv_matrix_location, 1, GL_FALSE, model_view_matrix);
  glUniform4f(u_ambient_color_location, ambient_color[0], ambient_color[1],
              ambient_color[2], ambient_color[3]);
  glUniform4f(u_diffuse_color_location, mat->diffuse_color[0], mat->diffuse_color[1],
              mat->diffuse_color[2], mat->diffuse_color[3]);
  glUniform3f(u_light_position_location, 0, 0, 5);
  glUniform1f(u_diffuse_intensity_location, mat->diffuse_intensity);

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  // disable arrays
  glDisableVertexAttribArray(a_position_location);
  glDisableVertexAttribArray(a_normal_location);
}

void _draw_object_fw(object *o) {
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    _draw_mesh(o, o->meshes[i], o->mats[i]);
  }
}
