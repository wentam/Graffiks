#include "mesh.h"

mesh* create_mesh(float **vertices, int ***indicies, int index_count, float **normals) {
    float system_vertices[(index_count*3)*3];
    float system_normals[(index_count*3)*3];
    _generate_mesh(system_vertices, system_normals, vertices, indicies, index_count, normals);

    mesh *m = _allocate_mesh(index_count);

    glGenBuffers(1, &m->triangle_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(system_vertices), system_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m->normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(system_normals), system_normals, GL_STATIC_DRAW);
    return m;
}

mesh* create_mesh_st(float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]) {
    float system_vertices[(index_count*3)*3];
    float system_normals[(index_count*3)*3];
    _generate_mesh_st(system_vertices, system_normals, vertices, indicies, index_count, normals);

    mesh *m = _allocate_mesh(index_count);

    glGenBuffers(1, &m->triangle_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(system_vertices), system_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m->normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(system_normals), system_normals, GL_STATIC_DRAW);
    return m;
}

base_mesh* create_base_mesh(float **vertices, int ***indicies, int index_count, float **normals) {
    float *system_vertices = malloc(sizeof(float)*(index_count*3*3));
    float *system_normals = malloc(sizeof(float)*(index_count*3*3));

    _generate_mesh(system_vertices, system_normals, vertices, indicies, index_count, normals);

    base_mesh *bmesh = malloc(sizeof(base_mesh));
    bmesh->vertex_count = index_count*3*3;
    bmesh->normal_count = index_count*3*3;
    bmesh->vertices = system_vertices;
    bmesh->normals = system_normals;

    return bmesh;
}

mesh* create_mesh_with_instances(base_mesh *bmesh, double instances[][3], int instance_count, bool use_z) {
    float system_vertices[bmesh->vertex_count*instance_count];
    float system_normals[bmesh->normal_count*instance_count];

    int start_index = 0;
    int start_index_normal = 0;
    int index_offset = 0;
    int i;
    int i2;
    for (i = 0; i < instance_count; i++) {
        // vertices
        for (i2 = 0; i2 < bmesh->vertex_count; i2++) {
            if (index_offset == 2 && use_z == false) {
                system_vertices[start_index+i2] = bmesh->vertices[i2];
            } else {
                system_vertices[start_index+i2] = bmesh->vertices[i2]+instances[i][index_offset];
            }

            index_offset++;
            if (index_offset >= 3) {
                index_offset = 0;
            }
        }

        start_index += bmesh->vertex_count;

        // normals
        for (i2 = 0; i2 < bmesh->normal_count; i2++) {
            system_normals[start_index_normal+i2] = bmesh->normals[i2];
        }

        start_index_normal += bmesh->normal_count;
    }

    mesh *smesh = malloc(sizeof(mesh));
    smesh->vertex_count = bmesh->vertex_count*instance_count;
    smesh->normal_count = bmesh->normal_count*instance_count;
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
    /*glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(bmesh->vertex_count*instance_count), system_vertices, GL_DYNAMIC_DRAW);*/

    /*glGenBuffers(1, smesh->normal_buffer);*/
    /*glBindBuffer(GL_ARRAY_BUFFER, *(smesh->normal_buffer));*/
    /*glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(bmesh->normal_count*instance_count), system_normals, GL_DYNAMIC_DRAW);*/

    return smesh;
}

void free_mesh(mesh *m) {
    glDeleteBuffers(1,&m->triangle_buffer);
    glDeleteBuffers(1,&m->normal_buffer);
    free(m);
}

void free_base_mesh(base_mesh *m) {
    free(m->vertices);
    free(m->normals);
    free(m);
}

void draw_mesh(mesh *m, material *mat) {
    GLuint *program = mat->program;
    glUseProgram(*program);

    // get GLSL variable locations
    GLint u_mvp_matrix_location = glGetUniformLocation(*program, "u_mvp_matrix"); // model*view*projection
    GLint u_mv_matrix_location = glGetUniformLocation(*program, "u_mv_matrix"); // model*view
    GLint u_ambient_color_location = glGetUniformLocation(*program, "u_ambient_color");
    GLint u_diffuse_color_location = glGetUniformLocation(*program, "u_diffuse_color");
    GLint u_diffuse_intensity_location = glGetUniformLocation(*program, "u_diffuse_intensity");
    GLint u_light_position_location = glGetUniformLocation(*program, "u_light_position");
    GLint a_position_location = glGetAttribLocation(*program, "a_position");
    GLint a_normal_location = glGetAttribLocation(*program, "a_normal");

    // add vertices
    glBindBuffer(GL_ARRAY_BUFFER, m->triangle_buffer);
    glEnableVertexAttribArray(a_position_location);
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // add normals
    glBindBuffer(GL_ARRAY_BUFFER, m->normal_buffer);
    glEnableVertexAttribArray(a_normal_location);
    glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // set up matrices
    float rotation_matrix[16];
    float model_matrix[16];
    float model_rotation_matrix[16];
    float model_view_matrix[16];
    float model_view_projection_matrix[16];

    create_identity_matrix(model_matrix);

    translate_matrix(model_matrix, m->location_x, m->location_y, m->location_z);

    set_matrix_rotation(rotation_matrix, m->angle, m->rot_x, m->rot_y, m->rot_z);
    multiply_matrices(model_rotation_matrix, model_matrix, rotation_matrix);

    multiply_matrices(model_view_matrix, view_matrix, model_rotation_matrix);
    multiply_matrices(model_view_projection_matrix, projection_matrix, model_view_matrix);

    // send our data to the shader program
    glUniformMatrix4fv(u_mvp_matrix_location, 1, GL_FALSE, model_view_projection_matrix);
    glUniformMatrix4fv(u_mv_matrix_location, 1, GL_FALSE, model_view_matrix);
    glUniform4f(u_ambient_color_location, ambient_color[0], ambient_color[1], ambient_color[2], ambient_color[3]);
    glUniform4f(u_diffuse_color_location,
            mat->diffuse_color[0], mat->diffuse_color[1], mat->diffuse_color[2], mat->diffuse_color[3]);
    glUniform3f(u_light_position_location,0,0,5);
    glUniform1f(u_diffuse_intensity_location, mat->diffuse_intensity);

    // draw it!
    glDrawArrays(GL_TRIANGLES, 0, m->vertex_count/3);

    // disable arrays
    glDisableVertexAttribArray(a_position_location);
    glDisableVertexAttribArray(a_normal_location);
}

mesh* _allocate_mesh(int index_count) {

    mesh *m = malloc(sizeof(mesh));
    m->vertex_count = (index_count*3)*3;
    m->normal_count = (index_count*3)*3;
    m->location_x = 0;
    m->location_y = 0;
    m->location_z = 0;
    m->angle = 0;
    m->rot_x = 0;
    m->rot_y = 0;
    m->rot_z = 1;

    return m;
}

void _generate_mesh(float output_vertices[], float output_normals[],
        float **vertices, int ***indicies, int index_count, float **normals) {

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
        float vertices[][3], int indicies[][3][3], int index_count, float normals[][3]) {

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
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks","--");
    for (i = 0; i < 4; i++) {
        int i2;
        float row[4];
        for (i2 = 0; i2 < 4; i2++) {
            row[i2] = m[(i*4)+i2];
        }
        __android_log_print(ANDROID_LOG_ERROR, "Graffiks","[%f, %f, %f, %f]",row[0],row[1],row[2],row[3]);
    }
    __android_log_print(ANDROID_LOG_ERROR, "Graffiks","--");
}
