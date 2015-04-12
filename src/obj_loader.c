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

int _resize_2d_int_array(float ***array, int previous_size, int size, int size2) {
    if (previous_size > size) {
        // we're shrinking, so free stuff
        int i;
        for (i = previous_size-1; i >= size; i--) {
            free((*array)[i]);
        }
    }

    *array = realloc(*array, size * sizeof(float *));

    if (size > previous_size) {
        // we're expanding, so allocate stuff
        int i;
        for (i = previous_size; i < size; i++) {
            (*array)[i] = malloc(size2*sizeof(float));
        }
    }

    return size;
}

void _free_2d_int_array(float ***array, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free((*array)[i]);
    }

    free(*array);
}

int _resize_faces(int ****faces, int previous_size, int size) {
    if (previous_size > size) {
        int i;
        for (i = previous_size-1; i >= size; i--) {
            free((*faces)[i][0]);
            free((*faces)[i][1]);
            free((*faces)[i][2]);

            free((*faces)[i]);
        }
    }

    *faces = realloc(*faces, size * sizeof(int**));

    if (size > previous_size) {
        int i;
        for (i = previous_size; i < size; i++) {
            (*faces)[i] = malloc(3*sizeof(int*));

            (*faces)[i][0] = malloc(3*sizeof(int));
            (*faces)[i][1] = malloc(3*sizeof(int));
            (*faces)[i][2] = malloc(3*sizeof(int));
        }
    }

    return size;
}

void _free_faces(int ****faces, int size) {
    int i;
    for (i=0; i < size; i++) {
        free((*faces)[i][0]);
        free((*faces)[i][1]);
        free((*faces)[i][2]);

        free((*faces)[i]);
    }

    free(*faces);
}

// Any obj you load must have:
// * normals
// * triangulated faces
//
// not all 3d software exports these by default.
mesh* load_obj(char *filepath) {
    int line_count;
    char **obj_data = _read_obj_data(filepath, &line_count);
    float **verts = NULL;
    int vertex_count = 0;
    int allocated_vertex_count;
    int ***faces = NULL;
    int face_count = 0;
    int allocated_face_count;
    float **normals = NULL;
    int normal_count = 0;
    int allocated_normal_count;

    allocated_vertex_count = _resize_2d_int_array(&verts, 0, 64, 3);
    allocated_face_count = _resize_faces(&faces, 0, 64);
    allocated_normal_count = _resize_2d_int_array(&normals, 0, 64, 3);

    int i;
    for (i = 0; i < line_count; i++) {
        if (obj_data[i][0] == 'v' && obj_data[i][1] != 't' && obj_data[i][1] != 'n') {
            char *ptr;
            strtok_r(obj_data[i], " ", &ptr);

            vertex_count++;

            if (vertex_count > allocated_vertex_count) {
                allocated_vertex_count =
                    _resize_2d_int_array(&verts, allocated_vertex_count, allocated_vertex_count+64, 3);
            }

            verts[vertex_count-1][0] = atof(strtok_r(NULL, " ", &ptr));
            verts[vertex_count-1][1] = atof(strtok_r(NULL, " ", &ptr));
            verts[vertex_count-1][2] = atof(strtok_r(NULL, " ", &ptr));
        }
        else if (obj_data[i][0] == 'f') {
            char *ptr;
            strtok_r(obj_data[i], " ", &ptr);
            char *first = strtok_r(NULL, " ", &ptr);
            char *second = strtok_r(NULL, " ", &ptr);
            char *third = strtok_r(NULL, " ", &ptr);

            face_count++;

            if (face_count > allocated_face_count) {
                allocated_face_count =
                    _resize_faces(&faces, allocated_face_count, allocated_face_count+64 );
            }

            char *ptr2;
            faces[face_count-1][0][0] = atoi(strtok_r(first, "/", &ptr2))-1;
            faces[face_count-1][0][1] = atoi(strtok_r(NULL, "/", &ptr2))-1;
            char *third_token = strtok_r(NULL, "/", &ptr2);

            if (third_token != NULL) {
                faces[face_count-1][0][2] = atoi(third_token)-1;
            } else {
                faces[face_count-1][0][2] = faces[face_count-1][0][1];
            }

            char *ptr3;
            faces[face_count-1][1][0] = atoi(strtok_r(second, "/", &ptr3))-1;
            faces[face_count-1][1][1] = atoi(strtok_r(NULL, "/", &ptr3))-1;

            third_token = strtok_r(NULL, "/", &ptr3);
            if (third_token != NULL) {
                faces[face_count-1][1][2] = atoi(third_token)-1;
            } else {
                faces[face_count-1][1][2] = faces[face_count-1][1][1];
            }

            char *ptr4;
            faces[face_count-1][2][0] = atoi(strtok_r(third, "/", &ptr4))-1;
            faces[face_count-1][2][1] = atoi(strtok_r(NULL, "/", &ptr4))-1;

            third_token = strtok_r(NULL, "/", &ptr4);
            if (third_token != NULL) {
                faces[face_count-1][2][2] = atoi(third_token)-1;
            } else {
                faces[face_count-1][2][2] = faces[face_count-1][2][1];
            }
        }
        else if (obj_data[i][0] == 'v' && obj_data[i][1] == 'n') {
            char *ptr;
            strtok_r(obj_data[i], " ", &ptr);

            normal_count++;

            if (normal_count > allocated_normal_count) {
                allocated_normal_count =
                    _resize_2d_int_array(&normals, allocated_normal_count, allocated_normal_count+64, 3);
            }

            normals[normal_count-1][0] = atof(strtok_r(NULL, " ", &ptr));
            normals[normal_count-1][1] = atof(strtok_r(NULL, " ", &ptr));
            normals[normal_count-1][2] = atof(strtok_r(NULL, " ", &ptr));
        }
    }

    // shrink arrays to true size
    allocated_vertex_count = _resize_2d_int_array(&verts, allocated_vertex_count, vertex_count, 3);
    allocated_face_count = _resize_faces(&faces, allocated_face_count, face_count);
    allocated_normal_count = _resize_2d_int_array(&normals, allocated_normal_count, normal_count, 3);

    //printf("%f\n", normals[0][0]);

    // free obj data
    _free_obj_data(&obj_data, line_count);

    // create mesh
    mesh *m = create_mesh(verts, faces, face_count, normals);

    // free other stuff
    _free_2d_int_array(&verts, allocated_vertex_count);
    _free_faces(&faces, allocated_face_count);
    _free_2d_int_array(&normals, allocated_normal_count);

    return m;
}
