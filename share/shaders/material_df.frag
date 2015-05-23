#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 100) uniform vec3 u_light_position;

in vec4 v_ambient_color;
in vec4 v_diffuse_color;
in float v_diffuse_intensity;
in vec3 v_position;
in vec3 v_normal;

layout(location = 0) out vec4 out_diffuse;
layout(location = 1) out vec3 out_normal;

void main() {
  float distance = length(u_light_position - v_position);
  vec3 light_vector = normalize(u_light_position - v_position);

  /* calculate diffuse intensity for this fragment */
  float diffuse = max(dot(v_normal, light_vector), 0.1) * v_diffuse_intensity;
  diffuse = clamp(diffuse, 0.0, 9.0);
  diffuse = diffuse * (1.0 / ((0.7 * distance)));

  /* combine target color with diffuse intensity and ambient color. set as result. */
  //gl_FragColor = (v_diffuse_color * diffuse) + v_ambient_color;
  //out_diffuse = (v_diffuse_color * diffuse) + v_ambient_color;
  /*gl_FragColor = vec4(0.0,1.0,0.0,1.0);*/

  out_diffuse = v_diffuse_color;
  out_normal = v_normal;
}
