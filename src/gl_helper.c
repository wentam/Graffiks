#include "graffiks/gl_helper.h"
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

void gfks_set_view_matrix(float matrix[], float eye_x, float eye_y, float eye_z,
                          float center_x, float center_y, float center_z, float up_x,
                          float up_y, float up_z) {

  // the view matrix looks like this:
  //   x   y   z   t
  // | x | x | x | x |
  // | y | y | y | y |
  // | z | z | z | z |
  // | 0 | 0 | 0 | 1 |
  //
  // each column is a vector from 0,0,0 to the specified location.
  //
  // we need to define the columns x to be a vector to the right, y to be a
  // vector above, and z to be a vector forwards,
  // relative to the "camera"'s angle. these need to be normalized as we get
  // passed vectors of arbitrary length.
  // we also need to set the column t to the location of the camera.

  // the forward vector is the difference between what we're looking at and
  // where we are
  float forward_vector_x = center_x - eye_x;
  float forward_vector_y = center_y - eye_y;
  float forward_vector_z = center_z - eye_z;

  // normalize forward vector
  float vl = 0;
  gfks_vector_length(&vl, forward_vector_x, forward_vector_y, forward_vector_z);

  float nf = 1.0 / vl;
  forward_vector_x *= nf;
  forward_vector_y *= nf;
  forward_vector_z *= nf;

  // right is the vector product of forward and up
  float right_vector_x = forward_vector_y * up_z - forward_vector_z * up_y;
  float right_vector_y = forward_vector_z * up_x - forward_vector_x * up_z;
  float right_vector_z = forward_vector_x * up_y - forward_vector_y * up_x;

  // normalize the right vector
  gfks_vector_length(&vl, right_vector_x, right_vector_y, right_vector_z);
  float ns = 1.0 / vl;
  right_vector_x *= ns;
  right_vector_y *= ns;
  right_vector_z *= ns;

  // to get the correct mangnitude for up we just use the vector product of
  // right and forward
  float up_vector_x =
      right_vector_y * forward_vector_z - right_vector_z * forward_vector_y;
  float up_vector_y =
      right_vector_z * forward_vector_x - right_vector_x * forward_vector_z;
  float up_vector_z =
      right_vector_x * forward_vector_y - right_vector_y * forward_vector_x;

  // and now we write it all to the matrix
  //
  // note that we need to invert the forward vector, as we're moving the world
  // and the not the camera
  matrix[0] = right_vector_x;
  matrix[1] = up_vector_x;
  matrix[2] = -forward_vector_x;
  matrix[3] = 0;

  matrix[4] = right_vector_y;
  matrix[5] = up_vector_y;
  matrix[6] = -forward_vector_y;
  matrix[7] = 0;

  matrix[8] = right_vector_z;
  matrix[9] = up_vector_z;
  matrix[10] = -forward_vector_z;
  matrix[11] = 0;

  // just set up the translation vector as empty for now
  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;

  // now we'll translate.
  // we're moving the world and not the camera, so we invert all axes
  gfks_translate_matrix(matrix, -eye_x, -eye_y, -eye_z);
}

void gfks_set_projection_matrix(float matrix[], float left, float right, float top,
                                float bottom, float near, float far) {

  float r_width = 1 / (right - left);
  float r_height = 1 / (top - bottom);
  float r_depth = 1 / (near - far);
  float x = 2 * (near * r_width);
  float y = 2 * (near * r_height);
  float A = (right + left) * r_width;
  float B = (top + bottom) * r_height;
  float C = (far + near) * r_depth;
  float D = 2 * (far * near * r_depth);

  matrix[0] = x;
  matrix[1] = 0;
  matrix[2] = 0;
  matrix[3] = 0;
  matrix[4] = 0;
  matrix[5] = y;
  matrix[6] = 0;
  matrix[7] = 0;
  matrix[8] = A;
  matrix[9] = B;
  matrix[10] = C;
  matrix[11] = -1;
  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = D;
  matrix[15] = 0;
}

void gfks_create_identity_matrix(float m[]) {
  int i;
  for (i = 0; i < 16; i++) {
    m[i] = 0;
  }
  for (i = 0; i < 16; i += 5) {
    m[i] = 1;
  }
}

void gfks_translate_matrix(float matrix[], float x, float y, float z) {
  int i;
  for (i = 0; i < 4; i++) {
    matrix[12 + i] += matrix[i] * x + matrix[4 + i] * y + matrix[8 + i] * z;
  }
}

void gfks_set_matrix_rotation(float m[], float a, float x, float y, float z) {
  m[3] = 0;
  m[7] = 0;
  m[11] = 0;
  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;

  a *= M_PI / 180.0;
  float s = sin(a);
  float c = cos(a);

  if (x == 1.0 && y == 0.0 && z == 0.0) {
    m[5] = c;
    m[10] = c;
    m[6] = s;
    m[9] = -s;
    m[1] = 0;
    m[2] = 0;
    m[4] = 0;
    m[8] = 0;
    m[0] = 1;
  } else if (x == 0.0 && y == 1.0 && z == 0.0) {
    m[0] = c;
    m[10] = c;
    m[8] = s;
    m[2] = -s;
    m[1] = 0;
    m[4] = 0;
    m[6] = 0;
    m[9] = 0;
    m[5] = 1;
  } else if (x == 0.0 && y == 0.0 && z == 1.0) {
    m[0] = c;
    m[5] = c;
    m[1] = s;
    m[4] = -s;
    m[2] = 0;
    m[6] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 1;
  } else {
    float length;
    gfks_vector_length(&length, x, y, z);
    if (length != 1.0) {
      float r_len = 1 / length;
      x *= r_len;
      y *= r_len;
      z *= r_len;
    }
    float nc = 1.0 - c;
    float xy = x * y;
    float yz = y * z;
    float zx = z * x;
    float xs = x * s;
    float ys = y * s;
    float zs = z * s;
    m[0] = x * x * nc + c;
    m[4] = xy * nc - zs;
    m[8] = zx * nc + ys;
    m[1] = xy * nc + zs;
    m[5] = y * y * nc + c;
    m[9] = yz * nc - xs;
    m[2] = zx * nc - ys;
    m[6] = yz * nc + xs;
    m[10] = z * z * nc + c;
  }
}

void gfks_vector_length(float *result, float x, float y, float z) {
  *result = sqrt(x * x + y * y + z * z);
}

void gfks_matrix_element_minor(float *result, float m[], int matrix_size, int index) {
  int index_col_i = index / matrix_size;
  int index_row_i = index - (matrix_size * index_col_i);

  float *m2 =
      malloc(sizeof(float) * ((matrix_size * matrix_size) - (matrix_size * 2 - 1)));

  int m2_i = 0;
  int j;
  for (j = 0; j < matrix_size * matrix_size; j++) {
    int col_i = j / matrix_size;
    int row_i = j - (matrix_size * col_i);
    if (col_i != index_col_i && row_i != index_row_i) {
      m2[m2_i++] = m[j];
    }
  }

  gfks_matrix_determinant(result, m2, matrix_size - 1);
  free(m2);
}

void gfks_matrix_determinant(float *result, float m[], int matrix_size) {
  if (matrix_size == 1) {
    *result = m[0];
    return;
  }

  float final_result = 0.0f;
  short int alt = 0;
  int i;
  for (i = 0; i < matrix_size; i++) {
    float result2;
    gfks_matrix_element_minor(&result2, m, matrix_size, i * matrix_size);

    if (alt == 0) {
      final_result += m[i * matrix_size] * result2;
      alt = 1;
    } else {
      final_result -= m[i * matrix_size] * result2;
      alt = 0;
    }
  }

  *result = final_result;
}

void gfks_adjugate_matrix(float m[], int matrix_size) {
  // left side
  int i;
  for (i = 1; i < matrix_size; i++) {
    float tmp = m[i * matrix_size];

    m[i * matrix_size] = m[i];
    m[i] = tmp;
  }

  // right side
  int starting_elem = ((matrix_size - 1) * matrix_size) + 1;
  for (i = starting_elem; i < (matrix_size * matrix_size) - 1; i++) {
    int vi = i - (starting_elem - 1);
    int si = (vi * matrix_size) + (matrix_size - 1);

    float tmp = m[si];

    m[si] = m[i];
    m[i] = tmp;
  }
}

void gfks_matrix_inverse(float result[], float m[], int matrix_size) {
  float *matrix_of_minors = malloc(sizeof(float) * (matrix_size * matrix_size));

  int i;
  for (i = 0; i < matrix_size * matrix_size; i++) {
    float r;
    gfks_matrix_element_minor(&r, m, matrix_size, i);
    matrix_of_minors[i] = r;
  }

  short int alt = 0;
  for (i = 0; i < matrix_size * matrix_size; i++) {
    if (alt == 0) {
      alt = 1;
    } else {
      alt = 0;
      matrix_of_minors[i] = -matrix_of_minors[i];
    }
  }

  gfks_adjugate_matrix(matrix_of_minors, matrix_size);

  float original_determinant;
  gfks_matrix_determinant(&original_determinant, m, matrix_size);

  for (i = 0; i < matrix_size * matrix_size; i++) {
    matrix_of_minors[i] *= 1 / original_determinant;
  }

  for (i = 0; i < matrix_size * matrix_size; i++) {
    result[i] = matrix_of_minors[i];
  }

  free(matrix_of_minors);
}

void gfks_multiply_matrices(float result[], float m1[], float m2[]) {
  int r;
  for (r = 0; r < 4; r++) {
    int c;
    for (c = 0; c < 4; c++) {
      // result[r+(4*c)] = dot product of row r in m1 and column c
      result[r + (4 * c)] = 0;
      int i;
      for (i = 0; i < 4; i++) {
        result[r + (4 * c)] += m1[r + (i * 4)] * m2[i + (c * 4)];
      }
    }
  }
}
