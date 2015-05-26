#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec4 in_position;
layout(location = 1) uniform mat4 u_mvp_matrix;
layout(location = 2) uniform mat4 u_mv_matrix;
layout(location = 3) uniform vec3 u_ambient_color;

void main() {
  gl_Position = (u_mvp_matrix) * in_position;
}
