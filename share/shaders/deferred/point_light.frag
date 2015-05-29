#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 5) uniform vec3 u_light_position;
layout(location = 6) uniform vec2 u_renderer_size;
layout(location = 1) uniform sampler2D color_tex;
layout(location = 2) uniform sampler2D normals_tex;
layout(location = 3) uniform sampler2D position_tex;
layout(location = 7) uniform float specularity_intensity;
layout(location = 8) uniform float specularity_hardness;
layout(location = 9) uniform vec3 specularity_color;

in vec3 v_position;

void main() {
  vec2 frag_coord = gl_FragCoord.xy / u_renderer_size;

  vec3 diffuse_color = texture(color_tex, frag_coord).rgb;
  float diffuse_intensity = (texture(color_tex, frag_coord).a);
  vec3 position = texture(position_tex, frag_coord).rgb;
  vec3 normal = texture(normals_tex, frag_coord).rgb;

  float distance = length(u_light_position - position);
  vec3 light_vector = normalize(u_light_position - position);
  float attenuation = 1.0 / (0.7 * distance);
  vec3 view_direction = vec3(0.0,0.0,1.0);

  vec3 specular;
  vec3 diffuse;
  if (dot(normal, light_vector) < 0) {
    // light source is on the opposite side of the face
    specular = vec3(0.0,0.0,0.0);
    diffuse = vec3(0.0,0.0,0.0);
  } else {
    specular = attenuation * (specularity_color*specularity_intensity) * pow(max(0, dot(reflect(-light_vector, normal), view_direction)),specularity_hardness);
    diffuse = attenuation * diffuse_color * max(dot(normal, light_vector), 0) * diffuse_intensity;
  }

  gl_FragColor = vec4(specular+diffuse, 1.0);
}
