#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_diffuse_color;

layout(location = 3) uniform mat4 u_mvp_matrix;
layout(location = 4) uniform mat4 u_mv_matrix;
layout(location = 6) uniform vec4 u_diffuse_color;
layout(location = 8) uniform bool u_per_vertex;

out vec4 v_diffuse_color;
out float v_diffuse_intensity;
out vec3 v_position;
out vec3 v_normal;

void main() {
  if (u_per_vertex) {
      v_diffuse_color = in_diffuse_color;
  } else {
      v_diffuse_color = u_diffuse_color;
  }


  v_position = vec3((u_mv_matrix) * in_position);
  v_normal = vec3(u_mv_matrix * vec4(in_normal, 0.0));

  gl_Position = (u_mvp_matrix) * in_position;
}
