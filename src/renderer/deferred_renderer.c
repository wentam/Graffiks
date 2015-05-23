#include "renderer/deferred_renderer.h"
#include "governor.h"
#include "material/material.h"
#include "material/material_df.h"

GLuint color_tex, normals_tex, fbo;

void _init_renderer_df() {
  // diffuse
  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer_width, renderer_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  // normals
  glGenTextures(1, &normals_tex);
  glBindTexture(GL_TEXTURE_2D, normals_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer_width, renderer_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normals_tex, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
};

void _clear_df() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void _destroy_renderer_df() { glDeleteFramebuffers(1, &fbo); }

void _debug_show_fbo() {
  /* We are going to blit into the window (default framebuffer)                     */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK); /* Use backbuffer as color dst.         */

  /* Read from your FBO */
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT1); /* Use Color Attachment 0 as color src. */

  /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480,
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

// this is the "geometry" pass
void _df_draw_mesh(object *o, mesh *m, material *mat) {

  // draw into first pass buffers
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

  GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);

  GLuint *program = mat->program;
  glUseProgram(*program);

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
  glUniformMatrix4fv(GRAFFIKS_MATERIAL_DF_UATTRIB_MVP_MATRIX, 1, GL_FALSE,
                     model_view_projection_matrix);
  glUniformMatrix4fv(GRAFFIKS_MATERIAL_DF_UATTRIB_MV_MATRIX, 1, GL_FALSE,
                     model_view_matrix);
  glUniform4f(GRAFFIKS_MATERIAL_DF_UATTRIB_AMBIENT_COLOR, ambient_color[0],
              ambient_color[1], ambient_color[2], ambient_color[3]);
  glUniform4f(GRAFFIKS_MATERIAL_DF_UATTRIB_DIFFUSE_COLOR, mat->diffuse_color[0],
              mat->diffuse_color[1], mat->diffuse_color[2], mat->diffuse_color[3]);
  glUniform3f(GRAFFIKS_MATERIAL_DF_UATTRIB_LIGHT_POSITION, 0, 0, 5);
  glUniform1f(GRAFFIKS_MATERIAL_DF_UATTRIB_DIFFUSE_INTENSITY, mat->diffuse_intensity);

  // add vertices
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_POSITION);
  glVertexAttribPointer(GRAFFIKS_MATERIAL_DF_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0,
                        0);

  // add normals
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glEnableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_NORMAL);
  glVertexAttribPointer(GRAFFIKS_MATERIAL_DF_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add colors if m->use_vertex_color
  if (m->use_vertex_color) {
    glUniform1i(GRAFFIKS_MATERIAL_DF_UATTRIB_PER_VERTEX, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
    glEnableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_DIFFUSE_COLOR);
    glVertexAttribPointer(GRAFFIKS_MATERIAL_DF_ATTRIB_DIFFUSE_COLOR, 4, GL_FLOAT,
                          GL_FALSE, 0, 0);
  } else {
    glUniform1i(GRAFFIKS_MATERIAL_DF_UATTRIB_PER_VERTEX, 0);
  }

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  // disable arrays
  glDisableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_POSITION);
  glDisableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_NORMAL);

  _debug_show_fbo();
}

void _draw_object_df(object *o) {
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    _df_draw_mesh(o, o->meshes[i], o->mats[i]);
  }
}
