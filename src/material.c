#include "material.h"

#ifdef ANDROID
JNIEnv *env;
jobject asset_manager;
#endif

material* create_material() {
    #ifdef ANDROID
    AAssetManager* mgr = AAssetManager_fromJava(env, asset_manager);
    if (mgr == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","Failed to get asset manager");
    }

    // read vertex shader
    AAsset* asset = AAssetManager_open(mgr, "shaders/material.vert", AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","Vertex shader not found");
    }
    off_t vsize = AAsset_getLength(asset);
    char* vertex_shader = malloc(sizeof(char)*vsize);
    AAsset_read(asset,vertex_shader,vsize);
    AAsset_close(asset);

    // read fragment shader
    asset = AAssetManager_open(mgr, "shaders/material.frag", AASSET_MODE_UNKNOWN);

    if (asset == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","Fragment shader not found");
    }
    off_t fsize = AAsset_getLength(asset);
    char* fragment_shader = malloc(sizeof(char)*fsize);
    AAsset_read(asset,fragment_shader,fsize);
    AAsset_close(asset);
    #endif

    #ifdef LINUX
    // read vertex shader
    char *vertex_shader;
    long vertex_shader_file_size;
    FILE *vertex_shader_fp = fopen(RESOURCE_PATH "/shaders/material.vert", "r");
    fseek(vertex_shader_fp, 0, SEEK_END);
    vertex_shader_file_size = ftell(vertex_shader_fp);
    rewind(vertex_shader_fp);
    vertex_shader = malloc((vertex_shader_file_size+1) * sizeof(char));
    fread(vertex_shader, sizeof(char), vertex_shader_file_size, vertex_shader_fp);
    fclose(vertex_shader_fp);
    vertex_shader[vertex_shader_file_size] = 0;

    // read fragment shader
    char *fragment_shader;
    long fragment_shader_file_size;
    FILE *fragment_shader_fp = fopen(RESOURCE_PATH "/shaders/material.frag", "r");
    fseek(fragment_shader_fp, 0, SEEK_END);
    fragment_shader_file_size = ftell(fragment_shader_fp);
    rewind(fragment_shader_fp);
    fragment_shader = malloc((fragment_shader_file_size+1) * sizeof(char));
    fread(fragment_shader, sizeof(char), fragment_shader_file_size, fragment_shader_fp);
    fclose(fragment_shader_fp);

    int vsize = (int) vertex_shader_file_size;
    int fsize = (int) fragment_shader_file_size;
    #endif

    GLuint vertex_shader_ref = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ref = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader_ref, 1, (const GLchar**)&vertex_shader, (int*) &vsize);
    glShaderSource(fragment_shader_ref, 1, (const GLchar**)&fragment_shader, (int*) &fsize);

    glCompileShader(vertex_shader_ref);
    glCompileShader(fragment_shader_ref);

    GLint status;
    glGetShaderiv(vertex_shader_ref, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(vertex_shader_ref, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar info[info_log_length+1];
        glGetShaderInfoLog(vertex_shader_ref, info_log_length, NULL, info);

        #ifdef ANDROID
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","%s", info);
        #endif
    }

    glGetShaderiv(fragment_shader_ref, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(fragment_shader_ref, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar info[info_log_length+1];
        glGetShaderInfoLog(fragment_shader_ref, info_log_length, NULL, info);

        #ifdef ANDROID
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","%s", info);
        #endif
    }

    GLuint *program = malloc(sizeof(GLuint));
    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader_ref);
    glAttachShader(*program, fragment_shader_ref);
    glBindAttribLocation(*program, 0, "a_position");
    glBindAttribLocation(*program, 1, "a_normal");
    glLinkProgram(*program);

    free(fragment_shader);
    free(vertex_shader);

    float *diffuse_color = malloc(sizeof(float)*4);
    diffuse_color[0] = 1;
    diffuse_color[1] = 1;
    diffuse_color[2] = 1;
    diffuse_color[3] = 1;

    material *m = malloc(sizeof(material));
    m->program = program;
    m->diffuse_intensity = 10;
    m->diffuse_color = diffuse_color;


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
    float *dc = malloc(sizeof(float)*4);
    int i;
    for (i = 0; i < 4; i++) {
        dc[i] = diffuse_color[i];
    }
    /*dc = diffuse_color;*/
    m->diffuse_color = dc;
}

#ifdef ANDROID
void _set_material_sys(JNIEnv *e, jobject a) {
    env = e;
    asset_manager = a;
}
#endif
