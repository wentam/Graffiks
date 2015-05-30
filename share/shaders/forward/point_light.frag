#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 100) uniform vec3 u_light_position;
layout(location = 10) uniform float specularity_hardness;
layout(location = 11) uniform vec3 specularity_color;

in vec4 v_diffuse_color;
in vec3 v_position;
centroid in vec3 v_normal;

void main() {
  float distance = length(u_light_position - v_position);
  vec3 light_vector = normalize(u_light_position - v_position);
  float attenuation = 1.0 / (0.7 * distance);
  vec3 view_direction = vec3(0.0,0.0,1.0);

  vec4 specular;
  vec4 diffuse;
  if (dot(v_normal, light_vector) < 0) {
    // light source is on the opposite side of the face
    specular = vec4(0.0,0.0,0.0,1.0);
    diffuse = vec4(0.0,0.0,0.0,1.0);
  } else {
    specular = attenuation * vec4(specularity_color,1.0) * pow(max(0, dot(reflect(-light_vector, v_normal), view_direction)),specularity_hardness);
    diffuse = attenuation * v_diffuse_color * max(dot(v_normal, light_vector), 0);
  }

  /* combine target color with diffuse intensity and ambient color. set as result. */
  gl_FragColor = specular+diffuse;
}
