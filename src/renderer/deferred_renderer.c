#include "renderer/deferred_renderer.h"
#include "governor.h"
#include "material/material.h"
#include "material/material_df.h"
#include "mesh/plane_mesh.h"
#include "mesh/mesh.h"

GLuint color_tex, normals_tex, position_tex, depth_tex, ambient_tex, fbo;
GLuint *light_pass_program;

void _init_renderer_df() {
  // diffuse color (alpha is diffuse intensity)
  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer_width, renderer_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // ambient color
  glGenTextures(1, &ambient_tex);
  glBindTexture(GL_TEXTURE_2D, ambient_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer_width, renderer_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // normals
  glGenTextures(1, &normals_tex);
  glBindTexture(GL_TEXTURE_2D, normals_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer_width, renderer_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // position
  glGenTextures(1, &position_tex);
  glBindTexture(GL_TEXTURE_2D, position_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer_width, renderer_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // depth
  glGenTextures(1, &depth_tex);
  glBindTexture(GL_TEXTURE_2D, depth_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, renderer_width, renderer_height,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normals_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, position_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, ambient_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  light_pass_program = _create_program("/shaders/material_df_light.vert",
                                       "/shaders/material_df_light.frag");
};

void _clear_df() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void _destroy_renderer_df() { glDeleteFramebuffers(1, &fbo); }

void _debug_show_fbo() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT1);

  glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void _df_draw_mesh_geom_pass(object *o, mesh *m, material *mat) {

  // draw into first pass buffers
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

  GLuint attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
  glDrawBuffers(4, attachments);

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
}

void _light_pass_df() {
  //  glEnable(GL_BLEND);
  //  glBlendEquation(GL_FUNC_ADD);
  //  glBlendFunc(GL_ONE, GL_ONE);

  mesh *m = create_plane(2, 2); // should not be created every time we draw.
                                // Also remember to remove the mesh includes

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  glUseProgram(*light_pass_program);

  // bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normals_tex);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, position_tex);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, ambient_tex);

  // assign textures to uniforms
  glUniform1i(1, 0);
  glUniform1i(2, 1);
  glUniform1i(3, 2);
  glUniform1i(4, 3);
  glUniform3f(100, 0, 0, 5);
  glUniform2f(101, renderer_width, renderer_height);

  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(GRAFFIKS_MATERIAL_DF_ATTRIB_POSITION);
  glVertexAttribPointer(GRAFFIKS_MATERIAL_DF_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0,
                        0);

  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);
}

void _draw_object_df(object *o) {
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    _df_draw_mesh_geom_pass(o, o->meshes[i], o->mats[i]);
  }
}
