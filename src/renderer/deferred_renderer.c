#include <GL/glew.h>
#include "graffiks/renderer/deferred_renderer.h"
#include "graffiks/dt_loop.h"
#include "graffiks/material.h"
#include "graffiks/primitives.h"
#include "graffiks/mesh.h"
#include "graffiks/renderer/renderer.h"
#include "graffiks/lights.h"
#include "graffiks/camera.h"

GLuint color_tex, specular_tex, normals_tex, position_tex, depth_tex, ambient_tex, fbo;
GLuint *df_point_light_program;
GLuint *df_geom_pass_program;
GLuint *df_ambient_pass_program;

gfks_mesh *screen_mesh;

void _gfks_init_renderer_df() {
  // diffuse color
  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, renderer_width, renderer_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // specular r, g, b, hardness
  glGenTextures(1, &specular_tex);
  glBindTexture(GL_TEXTURE_2D, specular_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderer_width, renderer_height, 0, GL_RGBA,
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, renderer_width, renderer_height, 0, GL_RGB,
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
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, specular_tex, 0);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);

  GLuint attachments[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                           GL_COLOR_ATTACHMENT4};
  glDrawBuffers(5, attachments);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  df_ambient_pass_program = _gfks_create_program("/shaders/deferred/ambient_light.vert",
                                                 "/shaders/deferred/ambient_light.frag");
  df_point_light_program = _gfks_create_program("/shaders/deferred/point_light.vert",
                                                "/shaders/deferred/point_light.frag");
  df_geom_pass_program = _gfks_create_program("/shaders/deferred/geometry_pass.vert",
                                              "/shaders/deferred/geometry_pass.frag");

  screen_mesh = gfks_create_plane(2, 2);
};

void _gfks_clear_df() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void _gfks_terminate_renderer_df() { glDeleteFramebuffers(1, &fbo); }

void _gfks_debug_show_fbo() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT1);

  glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

const GLint GFKS_DF_GEOM_ATTRIB_POSITION = 0;
const GLint GFKS_DF_GEOM_ATTRIB_NORMAL = 1;
const GLint GFKS_DF_GEOM_ATTRIB_DIFFUSE_COLOR = 2;
const GLint GFKS_DF_GEOM_UATTRIB_MVP_MATRIX = 3;
const GLint GFKS_DF_GEOM_UATTRIB_MV_MATRIX = 4;
const GLint GFKS_DF_GEOM_UATTRIB_AMBIENT_COLOR = 5;
const GLint GFKS_DF_GEOM_UATTRIB_DIFFUSE_COLOR = 6;
const GLint GFKS_DF_GEOM_UATTRIB_SPEC_HARDNESS = 7;
const GLint GFKS_DF_GEOM_UATTRIB_PER_VERTEX = 8;
const GLint GFKS_DF_GEOM_UATTRIB_LIGHT_POSITION = 9;
const GLint GFKS_DF_GEOM_UATTRIB_SPEC_COLOR = 10;

void _gfks_df_draw_mesh_geom_pass(gfks_object *o, gfks_mesh *m, gfks_material *mat) {

  // draw into first pass buffers
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

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
                         mesh_rotation_matrix, 4);
  gfks_multiply_matrices(model_rotation_matrix, object_model_matrix,
                         object_rotation_matrix, 4);
  gfks_multiply_matrices(combined_model_rotation_matrix, model_rotation_matrix,
                         local_model_rotation_matrix, 4);

  gfks_multiply_matrices(model_view_matrix, gfks_view_matrix,
                         combined_model_rotation_matrix, 4);
  gfks_multiply_matrices(model_view_projection_matrix, gfks_projection_matrix,
                         model_view_matrix, 4);

  // send our data to the shader program
  glUniformMatrix4fv(GFKS_DF_GEOM_UATTRIB_MVP_MATRIX, 1, GL_FALSE,
                     model_view_projection_matrix);
  glUniformMatrix4fv(GFKS_DF_GEOM_UATTRIB_MV_MATRIX, 1, GL_FALSE, model_view_matrix);
  glUniform3f(GFKS_DF_GEOM_UATTRIB_AMBIENT_COLOR, gfks_ambient_color[0],
              gfks_ambient_color[1], gfks_ambient_color[2]);
  glUniform4f(GFKS_DF_GEOM_UATTRIB_DIFFUSE_COLOR, mat->diffuse_color[0],
              mat->diffuse_color[1], mat->diffuse_color[2], mat->diffuse_color[3]);
  glUniform3f(GFKS_DF_GEOM_UATTRIB_LIGHT_POSITION, 0, 0, 5);
  glUniform1f(GFKS_DF_GEOM_UATTRIB_SPEC_HARDNESS, mat->specularity_hardness);
  glUniform3f(GFKS_DF_GEOM_UATTRIB_SPEC_COLOR, mat->specularity_color_r,
              mat->specularity_color_g, mat->specularity_color_b);

  // add vertices
  glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
  glEnableVertexAttribArray(GFKS_DF_GEOM_ATTRIB_POSITION);
  glVertexAttribPointer(GFKS_DF_GEOM_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add normals
  glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
  glEnableVertexAttribArray(GFKS_DF_GEOM_ATTRIB_NORMAL);
  glVertexAttribPointer(GFKS_DF_GEOM_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // add colors if m->use_vertex_color
  if (m->use_vertex_color) {
    glUniform1i(GFKS_DF_GEOM_UATTRIB_PER_VERTEX, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertex_color_buffer);
    glEnableVertexAttribArray(GFKS_DF_GEOM_ATTRIB_DIFFUSE_COLOR);
    glVertexAttribPointer(GFKS_DF_GEOM_ATTRIB_DIFFUSE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    glUniform1i(GFKS_DF_GEOM_UATTRIB_PER_VERTEX, 0);
  }

  // draw it!
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count / 3);

  // disable arrays
  glDisableVertexAttribArray(GFKS_DF_GEOM_ATTRIB_POSITION);
  glDisableVertexAttribArray(GFKS_DF_GEOM_ATTRIB_NORMAL);
}

const GLint GFKS_DF_AMBIENT_ATTRIB_POSITION = 0;
const GLint GFKS_DF_AMBIENT_UATTRIB_AMBIENT_COLOR = 1;

void _gfks_ambient_pass_df() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDepthMask(GL_FALSE);
  glUseProgram(*df_ambient_pass_program);

  glUniform3f(GFKS_DF_AMBIENT_UATTRIB_AMBIENT_COLOR, gfks_ambient_color[0],
              gfks_ambient_color[1], gfks_ambient_color[2]);

  glBindBuffer(GL_ARRAY_BUFFER, screen_mesh->triangle_buffer);
  glEnableVertexAttribArray(GFKS_DF_AMBIENT_ATTRIB_POSITION);
  glVertexAttribPointer(GFKS_DF_AMBIENT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, screen_mesh->vertex_count / 3);
  glDepthMask(GL_TRUE);
}

const GLint GFKS_DF_POINT_LIGHT_ATTRIB_POSITION = 0;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_DIFFUSE_TEX = 1;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_NORMALS_TEX = 2;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_POSITION_TEX = 3;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_SPECULAR_TEX = 4;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_LIGHT_POSTION = 5;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_RENDERER_SIZE = 6;
const GLint GFKS_DF_POINT_LIGHT_UATTRIB_LIGHT_BRIGHTNESS = 7;

void _gfks_light_pass_point_df(gfks_point_light *light) {
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  // bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normals_tex);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, position_tex);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, specular_tex);

  glUniform1i(GFKS_DF_POINT_LIGHT_UATTRIB_DIFFUSE_TEX, 0);
  glUniform1i(GFKS_DF_POINT_LIGHT_UATTRIB_NORMALS_TEX, 1);
  glUniform1i(GFKS_DF_POINT_LIGHT_UATTRIB_POSITION_TEX, 2);
  glUniform1i(GFKS_DF_POINT_LIGHT_UATTRIB_SPECULAR_TEX, 3);
  glUniform3f(GFKS_DF_POINT_LIGHT_UATTRIB_LIGHT_POSTION, light->x, light->y, light->z);
  glUniform2f(GFKS_DF_POINT_LIGHT_UATTRIB_RENDERER_SIZE, renderer_width, renderer_height);
  glUniform1f(GFKS_DF_POINT_LIGHT_UATTRIB_LIGHT_BRIGHTNESS, light->brightness);

  glBindBuffer(GL_ARRAY_BUFFER, screen_mesh->triangle_buffer);
  glEnableVertexAttribArray(GFKS_DF_POINT_LIGHT_ATTRIB_POSITION);
  glVertexAttribPointer(GFKS_DF_POINT_LIGHT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, screen_mesh->vertex_count / 3);
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
}

void _gfks_geom_pass_df() {
  glUseProgram(*df_geom_pass_program);

  int i;
  for (i = 0; i < gfks_render_queue_size; i++) {
    if (gfks_render_queue[i]->material->renderer & GFKS_RENDERER_DEFERRED) {
      _gfks_df_draw_mesh_geom_pass(gfks_render_queue[i]->parent_object,
                                   gfks_render_queue[i]->mesh,
                                   gfks_render_queue[i]->material);
    }
  }
}

void _gfks_light_pass_df() {
  glUseProgram(*df_point_light_program);

  int i;
  for (i = 0; i < gfks_point_light_count; i++) {
    _gfks_light_pass_point_df(gfks_point_lights[i]);
  }
}

void _gfks_draw_object_df(gfks_object *o) {
  int i;
  for (i = 0; i < o->mesh_count; i++) {
    _gfks_df_draw_mesh_geom_pass(o, o->meshes[i], o->mats[i]);
  }
}
