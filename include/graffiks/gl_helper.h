#ifndef GL_HELPER_H
#define GL_HELPER_H
#include <math.h>
#ifdef ANDROID
#include <android/log.h>
#endif

// matrix is a float[16]
void set_view_matrix(float matrix[], float eye_x, float eye_y, float eye_z,
                     float center_x, float center_y, float center_z, float up_x,
                     float up_y, float up_z);

void set_projection_matrix(float matrix[], float left, float right, float top,
                           float bottom, float near, float far);

void create_identity_matrix(float m[]);
void translate_matrix(float matrix[], float x, float y, float z);
void set_matrix_rotation(float m[], float a, float x, float y, float z);
void vector_length(float *result, float x, float y, float z);
void multiply_matrices(float result[], float m1[], float m2[]);
void matrix_element_minor(float *result, float m[], int matrix_size, int index);
void matrix_determinant(float *result, float m[], int matrix_size);
void adjugate_matrix(float m[], int matrix_size);
void matrix_inverse(float result[], float m[], int matrix_size);
#endif