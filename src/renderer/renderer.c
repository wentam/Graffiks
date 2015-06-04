#include <GL/glew.h>
#include "graffiks/renderer/renderer.h"
#include "graffiks/renderer/deferred_renderer.h"
#include "graffiks/renderer/forward_renderer.h"

gfks_renderer_flags gfks_enabled_renderers = 0;
gfks_render_queue_item **gfks_render_queue;
int gfks_render_queue_size = 0;

void gfks_init_renderers(gfks_renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _gfks_init_renderer_df();
    gfks_enabled_renderers |= GRAFFIKS_RENDERER_DEFERRED;
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _gfks_init_renderer_fw();
    gfks_enabled_renderers |= GRAFFIKS_RENDERER_FORWARD;
  }
}

void gfks_terminate_renderers(gfks_renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _gfks_terminate_renderer_df();
    gfks_enabled_renderers &= ~GRAFFIKS_RENDERER_DEFERRED;
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _gfks_terminate_renderer_fw();
    gfks_enabled_renderers &= ~GRAFFIKS_RENDERER_FORWARD;
  }
}

void _gfks_clear(gfks_renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _gfks_clear_df();
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _gfks_clear_fw();
  }
}

#ifdef ANDROID
JNIEnv *env;
jobject asset_manager;
#endif

GLuint _gfks_create_shader(char *shader_filepath, int shader_type) {
#ifdef ANDROID
  AAssetManager *mgr = AAssetManager_fromJava(env, asset_manager);
  if (mgr == NULL) {
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "Failed to get asset manager");
  }

  // read vertex shader
  AAsset *asset = AAssetManager_open(mgr, shader_filepath, AASSET_MODE_UNKNOWN);
  if (asset == NULL) {
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "Shader not found");
  }
  off_t vsize = AAsset_getLength(asset);
  char *shader = malloc(sizeof(char) * vsize);
  AAsset_read(asset, shader, vsize);
  AAsset_close(asset);
#endif

#ifndef ANDROID
  // read shader
  char *shader;
  long shader_file_size;

  char *true_filepath = malloc(strlen(RESOURCE_PATH) + strlen(shader_filepath) + 1);
  strcpy(true_filepath, RESOURCE_PATH);
  strcat(true_filepath, shader_filepath);
  FILE *shader_fp = fopen(true_filepath, "r");
  if (!shader_fp) {
    printf("Unable to open resource file: %s\n", true_filepath);
  }
  free(true_filepath);

  fseek(shader_fp, 0, SEEK_END);
  shader_file_size = ftell(shader_fp);
  rewind(shader_fp);
  shader = malloc((shader_file_size + 1) * sizeof(char));
  size_t r_result = fread(shader, sizeof(char), shader_file_size, shader_fp);
  if (r_result != shader_file_size) {
    printf("error reading shader\n");
  }
  fclose(shader_fp);
  shader[shader_file_size] = 0;

  int vsize = (int)shader_file_size;
#endif

  GLuint shader_ref = glCreateShader(shader_type);
  glShaderSource(shader_ref, 1, (const GLchar **)&shader, (int *)&vsize);
  glCompileShader(shader_ref);
  free(shader);

  GLint status;
  glGetShaderiv(shader_ref, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint info_log_length;
    glGetShaderiv(shader_ref, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar *info = malloc((info_log_length + 1) * sizeof(GLchar));
    glGetShaderInfoLog(shader_ref, info_log_length, NULL, info);

    printf("Error compiling shader (%s) %s", shader_filepath, info);
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "%s", info);
#endif
    free(info);
  }

  return shader_ref;
}

GLuint *_gfks_create_program(char *vertex_shader_filepath,
                             char *fragment_shader_filepath) {
  GLuint vertex_shader_ref =
      _gfks_create_shader(vertex_shader_filepath, GL_VERTEX_SHADER);
  GLuint fragment_shader_ref =
      _gfks_create_shader(fragment_shader_filepath, GL_FRAGMENT_SHADER);

  GLuint *program = malloc(sizeof(GLuint));
  *program = glCreateProgram();
  glAttachShader(*program, vertex_shader_ref);
  glAttachShader(*program, fragment_shader_ref);
  glBindAttribLocation(*program, 0, "a_position");
  glBindAttribLocation(*program, 1, "a_normal");
  glLinkProgram(*program);

  return program;
}

#ifdef ANDROID
void _gfks_set_material_sys(JNIEnv *e, jobject a) {
  env = e;
  asset_manager = a;
}
#endif
