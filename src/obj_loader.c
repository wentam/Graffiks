#include "obj_loader.h"

int _resize_obj_data(char ***obj_data, int previous_size, int size) {
    if (previous_size > size) {
        int i;
        for (i = previous_size-1; i >= size; i--) {
            free((*obj_data)[i]);
        }
    }

    *obj_data = realloc(*obj_data, size * sizeof(char *));

    if (size > previous_size) {
        int i;
        for (i = previous_size; i < size; i++) {
            (*obj_data)[i] = malloc(256*sizeof(char));
        }
    }

    return size;
}

void _free_obj_data(char ***obj_data, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free((*obj_data)[i]);
    }

    free(*obj_data);
}

char** _read_obj_data(char *filepath, int *line_count_output) {
    int line_count = 0;
    char **obj_data = NULL;
    line_count = _resize_obj_data(&obj_data, 0, 64);

    FILE *obj_fp = fopen(filepath, "r");
    int current_line = 0;

    while (fgets(obj_data[current_line], sizeof(char)*256, obj_fp)) {
        current_line++;

        if (current_line >= line_count)  {
           line_count = _resize_obj_data(&obj_data, line_count, line_count+64);
        }
    }

    fclose(obj_fp);

    line_count = _resize_obj_data(&obj_data, line_count, current_line);

    *line_count_output = line_count;
    return obj_data;
}

int _resize_verts(float ***verts, int previous_size, int size) {
    if (previous_size > size) {
        // we're shrinking, so free stuff
        int i;
        for (i = previous_size-1; i >= size; i--) {
            free((*verts)[i]);
        }
    }

    *verts = realloc(*verts, size * sizeof(float *));

    if (size > previous_size) {
        // we're expanding, so allocate stuff
        int i;
        for (i = previous_size; i < size; i++) {
            (*verts)[i] = malloc(3*sizeof(float));
        }
    }

    return size;
}

void _free_verts(float ***verts, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free((*verts)[i]);
    }

    free(*verts);
}

int _resize_faces(int ****faces, int previous_size, int size) {

}

mesh* load_obj(char *filepath) {
    int line_count;
    char **obj_data = _read_obj_data(filepath, &line_count);
    float **verts = NULL;
    int ***faces = NULL;
    int vertex_count = 0;
    int allocated_vertex_count;
    allocated_vertex_count = _resize_verts(&verts, 0, 64);

    int i;
    for (i = 0; i < line_count; i++) {

        if (obj_data[i][0] == 'v' && obj_data[i][1] != 't') {
            char *ptr;
            strtok_r(obj_data[i], " ", &ptr);

            vertex_count++;

            if (vertex_count > allocated_vertex_count) {
                allocated_vertex_count =
                    _resize_verts(&verts, allocated_vertex_count, allocated_vertex_count+64);
            }

            verts[vertex_count-1][0] = atof(strtok_r(NULL, " ", &ptr));
            verts[vertex_count-1][1] = atof(strtok_r(NULL, " ", &ptr));
            verts[vertex_count-1][2] = atof(strtok_r(NULL, " ", &ptr));
        }
    }

    // resize verts array to true size
    allocated_vertex_count = _resize_verts(&verts, allocated_vertex_count, vertex_count);

    // free obj data
    _free_obj_data(&obj_data, line_count);

    // free verts
    _free_verts(&verts, allocated_vertex_count);

    return NULL;
}
