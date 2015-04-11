#include "obj_loader.h"

mesh* load_obj(char *filepath) {
    // read entire file to memory
    char *obj_data;

    long file_size;
    FILE *obj_fp = fopen(filepath, "r");
    fseek(obj_fp, 0, SEEK_END);
    file_size = ftell(obj_fp);
    rewind(obj_fp);
    obj_data = malloc((file_size+1)*sizeof(char));
    fread(obj_data, sizeof(char), file_size, obj_fp);
    fclose(obj_fp);

    // free obj file
    free(obj_data);
}
