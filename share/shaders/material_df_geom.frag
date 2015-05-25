#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 100) uniform vec3 u_light_position;
layout(location = 7) uniform float u_diffuse_intensity;
layout(location = 5) uniform vec4 u_ambient_color;

in vec4 v_diffuse_color;
in vec3 v_position;
in vec3 v_normal;

layout(location = 0) out vec4 out_diffuse;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_position;
layout(location = 3) out vec3 out_ambient;

void main() {
  float distance = length(u_light_position - v_position);
  vec3 light_vector = normalize(u_light_position - v_position);

  out_diffuse = vec4(v_diffuse_color.rgb, u_diffuse_intensity/100);
  out_normal = normalize(v_normal);
  out_position = v_position;
  out_ambient = u_ambient_color.rgb;
}
