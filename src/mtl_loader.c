#include "mtl_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int _string_starts_with_(char *pre, char *str)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

int _resize_mtl_data(char ***mtl_data, int previous_size, int size) {
    if (previous_size > size) {
        int i;
        for (i = previous_size-1; i >= size; i--) {
            free((*mtl_data)[i]);
        }
    }

    *mtl_data = realloc(*mtl_data, size * sizeof(char *));

    if (size > previous_size) {
        int i;
        for (i = previous_size; i < size; i++) {
            (*mtl_data)[i] = malloc(256*sizeof(char));
        }
    }

    return size;
}

char** _read_mtl_data(char *filepath, int *line_count_output) {
    char **mtl_data = NULL;
    int line_count = _resize_mtl_data(&mtl_data, 0, 64);


    FILE *obj_fp = fopen(filepath, "r");
    int current_line = 0;

    while (fgets(mtl_data[current_line], sizeof(char)*256, obj_fp)) {
        current_line++;

        if (current_line >= line_count)  {
           line_count = _resize_mtl_data(&mtl_data, line_count, line_count+64);
        }
    }

    fclose(obj_fp);

    line_count = _resize_mtl_data(&mtl_data, line_count, current_line);

    *line_count_output = line_count;
    return mtl_data;
}

int _resize_named_mats(named_material ***n, int previous_size, int size) {
    if (previous_size > size) {
        int i;
        for (i = previous_size-1; i > size; i--) {
            free((*n)[i]);
        }
    }

    *n = realloc(*n, size * sizeof(named_material*));

    if (size > previous_size) {
        int i;
        for (i = previous_size; i < size; i++) {
            (*n)[i] = malloc(sizeof(named_material));
            (*n)[i]->name = "";
        }
    }

    return size;
}

void free_named_mats(named_material_array *n) {
    int i;
    for (i = 0; i < n->number_of_mats; i++) {
        free(n->mats[i]->name);
        free(n->mats[i]);
    }

    free(n->mats);
    free(n);
}

named_material_array* load_mtl(char *filepath) {
    int line_count;
    char **mtl_data = _read_mtl_data(filepath, &line_count);

    named_material **mats = NULL;
    int allocated_mats = 0;
    int material_count = 0;

    int i;
    for (i = 0; i < line_count; i++) {
        if (_string_starts_with_("newmtl",mtl_data[i])) {
            char *ptr;
            strtok_r(mtl_data[i], " ", &ptr);
            char *material_name = strtok_r(NULL, " ", &ptr);
            char *material_name_heap = malloc(sizeof(char)*strlen(material_name));
            strcpy(material_name_heap, material_name);

            material_count++;
            allocated_mats = _resize_named_mats(&mats, allocated_mats, material_count);
            mats[material_count-1]->mat = create_material();
            mats[material_count-1]->name = material_name_heap;
        } else if (_string_starts_with_("Kd",mtl_data[i])) {
            char *ptr;
            strtok_r(mtl_data[i], " ", &ptr);

            float R = atof(strtok_r(NULL, " ", &ptr));
            float G = atof(strtok_r(NULL, " ", &ptr));
            float B = atof(strtok_r(NULL, " ", &ptr));


            float color[] = {R,G,B,1.0};
            set_diffuse_color(mats[material_count-1]->mat, color);
        }
    }

    named_material_array *arr = malloc(sizeof(named_material_array));
    arr->mats = mats;
    arr->number_of_mats = material_count;

    return arr;
}
