#include "graffiks/model_loaders/obj_loader.h"
#include "graffiks/model_loaders/mtl_loader.h"

#ifndef _WIN32
#include <libgen.h>
#endif

#ifdef _WIN32
#include <windows.h>
#define strtok_r strtok_s

char *dirname(const char *path) {
  char *out = malloc(strlen(path) + 1);
  _splitpath(path, NULL, out, NULL, NULL);
  if (out[0] == '\0') {
    out[0] = '.';
    out[1] = '\0';
  }
  return out;
}

#endif

int _gfks_string_starts_with(char *pre, char *str) {
  size_t lenpre = strlen(pre);
  size_t lenstr = strlen(str);
  return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

int _gfks_resize_obj_data(char ***obj_data, int previous_size, int size) {
  if (previous_size > size) {
    int i;
    for (i = previous_size - 1; i >= size; i--) {
      free((*obj_data)[i]);
    }
  }

  *obj_data = realloc(*obj_data, size * sizeof(char *));

  if (size > previous_size) {
    int i;
    for (i = previous_size; i < size; i++) {
      (*obj_data)[i] = malloc(256 * sizeof(char));
    }
  }

  return size;
}

void _gfks_free_obj_data(char ***obj_data, int size) {
  int i;
  for (i = 0; i < size; i++) {
    free((*obj_data)[i]);
  }

  free(*obj_data);
}

char **_gfks_read_obj_data(char *filepath, int *line_count_output) {
  char **obj_data = NULL;
  int line_count = _gfks_resize_obj_data(&obj_data, 0, 64);

  FILE *obj_fp = fopen(filepath, "r");
  int current_line = 0;

  while (fgets(obj_data[current_line], sizeof(char) * 256, obj_fp)) {
    current_line++;

    if (current_line >= line_count) {
      line_count = _gfks_resize_obj_data(&obj_data, line_count, line_count + 64);
    }
  }

  fclose(obj_fp);

  line_count = _gfks_resize_obj_data(&obj_data, line_count, current_line);

  *line_count_output = line_count;
  return obj_data;
}

int _gfks_resize_2d_int_array(float ***array, int previous_size, int size, int size2) {
  if (previous_size > size) {
    // we're shrinking, so free stuff
    int i;
    for (i = previous_size - 1; i >= size; i--) {
      free((*array)[i]);
    }
  }

  *array = realloc(*array, size * sizeof(float *));

  if (size > previous_size) {
    // we're expanding, so allocate stuff
    int i;
    for (i = previous_size; i < size; i++) {
      (*array)[i] = malloc(size2 * sizeof(float));
    }
  }

  return size;
}

void _gfks_free_2d_int_array(float ***array, int size) {
  int i;
  for (i = 0; i < size; i++) {
    free((*array)[i]);
  }

  free(*array);
}

int _gfks_resize_faces(int ****faces, int previous_size, int size) {
  if (previous_size > size) {
    int i;
    for (i = previous_size - 1; i >= size; i--) {
      free((*faces)[i][0]);
      free((*faces)[i][1]);
      free((*faces)[i][2]);

      free((*faces)[i]);
    }
  }

  *faces = realloc(*faces, size * sizeof(int **));

  if (size > previous_size) {
    int i;
    for (i = previous_size; i < size; i++) {
      (*faces)[i] = malloc(3 * sizeof(int *));

      (*faces)[i][0] = malloc(3 * sizeof(int));
      (*faces)[i][1] = malloc(3 * sizeof(int));
      (*faces)[i][2] = malloc(3 * sizeof(int));
    }
  }

  return size;
}

void _gfks_free_faces(int ****faces, int size) {
  int i;
  for (i = 0; i < size; i++) {
    free((*faces)[i][0]);
    free((*faces)[i][1]);
    free((*faces)[i][2]);

    free((*faces)[i]);
  }

  free(*faces);
}

/// \cond INTERNAL
typedef struct {
  int ***faces;
  int face_count;
  int allocated_face_count;
  gfks_material *mat;
} face_group;
/// \endcond

int _gfks_resize_face_groups(face_group ***f, int previous_size, int size) {
  if (previous_size > size) {
    int i;
    for (i = previous_size - 1; i >= size; i--) {
      free((*f)[i]);
    }
  }

  *f = realloc(*f, size * sizeof(face_group *));

  if (size > previous_size) {
    int i;
    for (i = previous_size; i < size; i++) {
      (*f)[i] = malloc(sizeof(face_group));
      (*f)[i]->faces = NULL;
      (*f)[i]->mat = NULL;
      (*f)[i]->face_count = 0;
    }
  }

  return size;
}

void _gfks_free_face_groups(face_group ***f, int size) {
  int i;
  for (i = 0; i < size; i++) {
    free((*f)[i]);
  }

  free(*f);
}

// Any obj you load must have:
// * normals
// * triangulated faces
// * usemtl statements and mtllib statement before all usemtl statements
// not all 3d software exports these by default.
gfks_object *gfks_load_obj(gfks_renderer_flags flags, char *filepath) {
  int line_count;
  char **obj_data = _gfks_read_obj_data(filepath, &line_count);
  float **verts = NULL;
  int vertex_count = 0;
  int allocated_vertex_count;

  face_group **face_groups = NULL;
  int mat_count = 0;
  int allocated_face_group_count = 0;

  float **normals = NULL;
  int normal_count = 0;
  int allocated_normal_count;

  gfks_named_material_array *mats = NULL;

  allocated_vertex_count = _gfks_resize_2d_int_array(&verts, 0, 64, 3);
  allocated_normal_count = _gfks_resize_2d_int_array(&normals, 0, 64, 3);

  int i;
  for (i = 0; i < line_count; i++) {
    if (obj_data[i][0] == 'v' && obj_data[i][1] != 't' && obj_data[i][1] != 'n') {
      char *ptr;
      strtok_r(obj_data[i], " ", &ptr);

      vertex_count++;

      if (vertex_count > allocated_vertex_count) {
        allocated_vertex_count = _gfks_resize_2d_int_array(
            &verts, allocated_vertex_count, allocated_vertex_count + 64, 3);
      }

      verts[vertex_count - 1][0] = atof(strtok_r(NULL, " ", &ptr));
      verts[vertex_count - 1][1] = atof(strtok_r(NULL, " ", &ptr));
      verts[vertex_count - 1][2] = atof(strtok_r(NULL, " ", &ptr));
    } else if (obj_data[i][0] == 'f') {
      int fi = mat_count - 1;

      char *ptr;
      strtok_r(obj_data[i], " ", &ptr);
      char *first = strtok_r(NULL, " ", &ptr);
      char *second = strtok_r(NULL, " ", &ptr);
      char *third = strtok_r(NULL, " ", &ptr);

      face_groups[fi]->face_count++;

      if (face_groups[fi]->face_count > face_groups[fi]->allocated_face_count) {
        face_groups[fi]->allocated_face_count = _gfks_resize_faces(
            &face_groups[fi]->faces, face_groups[fi]->allocated_face_count,
            face_groups[fi]->allocated_face_count + 64);
      }

      int i2;
      for (i2 = 0; i2 < 3; i2++) {
        char *ptr2;

        int first_token = 0;

        if (i2 == 0) {
          first_token = atoi(strtok_r(first, "/", &ptr2)) - 1;
        } else if (i2 == 1) {
          first_token = atoi(strtok_r(second, "/", &ptr2)) - 1;
        } else if (i2 == 2) {
          first_token = atoi(strtok_r(third, "/", &ptr2)) - 1;
        }

        face_groups[fi]->faces[face_groups[fi]->face_count - 1][i2][0] = first_token;
        face_groups[fi]->faces[face_groups[fi]->face_count - 1][i2][1] =
            atoi(strtok_r(NULL, "/", &ptr2)) - 1;
        char *third_token = strtok_r(NULL, "/", &ptr2);

        if (third_token != NULL) {
          face_groups[fi]->faces[face_groups[fi]->face_count - 1][i2][2] =
              atoi(third_token) - 1;
        } else {
          face_groups[fi]->faces[face_groups[fi]->face_count - 1][i2][2] =
              face_groups[fi]->faces[face_groups[fi]->face_count - 1][i2][1];
        }
      }
    } else if (obj_data[i][0] == 'v' && obj_data[i][1] == 'n') {
      char *ptr;
      strtok_r(obj_data[i], " ", &ptr);

      normal_count++;

      if (normal_count > allocated_normal_count) {
        allocated_normal_count = _gfks_resize_2d_int_array(
            &normals, allocated_normal_count, allocated_normal_count + 64, 3);
      }

      normals[normal_count - 1][0] = atof(strtok_r(NULL, " ", &ptr));
      normals[normal_count - 1][1] = atof(strtok_r(NULL, " ", &ptr));
      normals[normal_count - 1][2] = atof(strtok_r(NULL, " ", &ptr));
    } else if (_gfks_string_starts_with("usemtl", obj_data[i])) {
      char *ptr;
      strtok_r(obj_data[i], " ", &ptr);
      char *material_name = strtok_r(NULL, " ", &ptr);

      mat_count++;
      allocated_face_group_count =
          _gfks_resize_face_groups(&face_groups, allocated_face_group_count, mat_count);
      face_groups[mat_count - 1]->allocated_face_count =
          _gfks_resize_faces(&face_groups[mat_count - 1]->faces, 0, 64);

      int i2;
      for (i2 = 0; i2 < mats->number_of_mats; i2++) {
        if (strcmp(mats->mats[i2]->name, material_name) == 0) {
          face_groups[mat_count - 1]->mat = mats->mats[i2]->mat;
        }
      }
    } else if (_gfks_string_starts_with("mtllib", obj_data[i])) {
      char *ptr = NULL;
      strtok_r(obj_data[i], " ", &ptr);
      char *mtl_file = strtok_r(NULL, " ", &ptr);
      mtl_file[strlen(mtl_file) - 1] = 0;

      char *filepath_heap = malloc(sizeof(char) * (strlen(filepath) + 1));
      strcpy(filepath_heap, filepath);

      char *mtl_path = dirname(filepath_heap);
      char *mtl_filepath = malloc(strlen(mtl_path) + 2 + strlen(mtl_file) * sizeof(char));
      strcpy(mtl_filepath, mtl_path);
      strcat(mtl_filepath, "/");
      strcat(mtl_filepath, mtl_file);

      free(filepath_heap);

      mats = gfks_load_mtl(flags, mtl_filepath);

      free(mtl_filepath);
#ifdef _WIN32
      free(mtl_path);
#endif
    }
  }

  // shrink arrays to true size
  allocated_vertex_count =
      _gfks_resize_2d_int_array(&verts, allocated_vertex_count, vertex_count, 3);

  for (i = 0; i < mat_count; i++) {
    face_groups[i]->allocated_face_count =
        _gfks_resize_faces(&face_groups[i]->faces, face_groups[i]->allocated_face_count,
                           face_groups[i]->face_count);
  }
  allocated_normal_count =
      _gfks_resize_2d_int_array(&normals, allocated_normal_count, normal_count, 3);

  // free obj data
  _gfks_free_obj_data(&obj_data, line_count);

  // create object
  gfks_mesh **object_meshes = malloc(sizeof(gfks_mesh *) * allocated_face_group_count);
  gfks_material **object_mats =
      malloc(sizeof(gfks_material *) * allocated_face_group_count);
  for (i = 0; i < allocated_face_group_count; i++) {
    object_meshes[i] = gfks_create_mesh(verts, face_groups[i]->faces,
                                        face_groups[i]->face_count, normals);

    if (face_groups[i]->mat == NULL) {
      face_groups[i]->mat = gfks_create_material(flags);
    }

    object_mats[i] = face_groups[i]->mat;
  }
  gfks_object *o =
      gfks_create_object(object_meshes, object_mats, allocated_face_group_count);

  // free other stuff
  _gfks_free_2d_int_array(&verts, allocated_vertex_count);
  _gfks_free_2d_int_array(&normals, allocated_normal_count);
  gfks_free_named_mats(mats);

  for (i = 0; i < mat_count; i++) {
    _gfks_free_faces(&face_groups[i]->faces, face_groups[i]->allocated_face_count);
  }
  _gfks_free_face_groups(&face_groups, allocated_face_group_count);

  return o;
}
