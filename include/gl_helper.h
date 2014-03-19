#ifndef GL_HELPER_H
#define GL_HELPER_H
#include <math.h>
#include <android/log.h>

// matrix is a float[16]
void set_view_matrix (float matrix[],
                      float eye_x, float eye_y, float eye_z,
                      float center_x, float center_y, float center_z,
                      float up_x, float up_y, float up_z);

void set_projection_matrix(float matrix[],
                           float left, float right,
                           float top, float bottom,
                           float near, float far);

void create_identity_matrix(float m[]);
void translate_matrix(float matrix[], float x, float y, float z);
void set_matrix_rotation(float m[], float a, float x, float y, float z);
void vector_length(float *result, float x, float y, float z);
void multiply_matrices(float result[], float m1[], float m2[]);
#endif
