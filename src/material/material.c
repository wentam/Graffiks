#include "material/material.h"

#ifdef ANDROID
JNIEnv *env;
jobject asset_manager;
#endif

GLuint _create_shader(char *shader_filepath, int shader_type) {
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

#ifdef LINUX
  // read shader
  char *shader;
  long shader_file_size;

  char *true_filepath = malloc(strlen(RESOURCE_PATH) + strlen(shader_filepath) + 1);
  strcpy(true_filepath, RESOURCE_PATH);
  strcat(true_filepath, shader_filepath);
  FILE *shader_fp = fopen(true_filepath, "r");
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

    GLchar info[info_log_length + 1];
    glGetShaderInfoLog(shader_ref, info_log_length, NULL, info);

    printf("Error compiling shader (%s) %s", shader_filepath, info);
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks", "%s", info);
#endif
  }

  return shader_ref;
}

GLuint *_create_program(char *vertex_shader_filepath, char *fragment_shader_filepath) {
  GLuint vertex_shader_ref = _create_shader(vertex_shader_filepath, GL_VERTEX_SHADER);
  GLuint fragment_shader_ref =
      _create_shader(fragment_shader_filepath, GL_FRAGMENT_SHADER);

  GLuint *program = malloc(sizeof(GLuint));
  *program = glCreateProgram();
  glAttachShader(*program, vertex_shader_ref);
  glAttachShader(*program, fragment_shader_ref);
  glBindAttribLocation(*program, 0, "a_position");
  glBindAttribLocation(*program, 1, "a_normal");
  glLinkProgram(*program);

  return program;
}

material *create_material(renderer_flags flags) {
  GLuint *program;
  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    program = _create_program("/shaders/material.vert", "/shaders/material.frag");
  }

  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    program = _create_program("/shaders/material_df_geom.vert",
                              "/shaders/material_df_geom.frag");
  }

  float *diffuse_color = malloc(sizeof(float) * 4);
  diffuse_color[0] = 1;
  diffuse_color[1] = 1;
  diffuse_color[2] = 1;
  diffuse_color[3] = 1;

  material *m = malloc(sizeof(material));
  m->program = program;
  m->diffuse_intensity = 10;
  m->diffuse_color = diffuse_color;
  m->renderer = flags;

  return m;
}

void free_material(material *m) {
  free(m->program);
  free(m->diffuse_color);
  free(m);
}

void set_diffuse_intensity(material *m, float intensity) {
  m->diffuse_intensity = intensity;
}

void set_diffuse_color(material *m, float diffuse_color[]) {
  float *dc = malloc(sizeof(float) * 4);
  int i;
  for (i = 0; i < 4; i++) {
    dc[i] = diffuse_color[i];
  }
  m->diffuse_color = dc;
}

#ifdef ANDROID
void _set_material_sys(JNIEnv *e, jobject a) {
  env = e;
  asset_manager = a;
}
#endif
