#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 100) uniform vec3 u_light_position;
layout(location = 101) uniform vec2 u_renderer_size;
layout(location = 1) uniform sampler2D color_tex;
layout(location = 2) uniform sampler2D normals_tex;
layout(location = 3) uniform sampler2D position_tex;
layout(location = 4) uniform sampler2D ambient_tex;

in vec3 v_position;

void main() {
  vec2 frag_coord = gl_FragCoord.xy / u_renderer_size;

  vec3 diffuse_color = texture(color_tex, frag_coord).rgb;
  float diffuse_intensity = (texture(color_tex, frag_coord).a) * 100;
  vec3 position = texture(position_tex, frag_coord).rgb;
  vec3 normal = texture(normals_tex, frag_coord).rgb;
  vec3 ambient = texture(ambient_tex, frag_coord).rgb;

  float distance = length(u_light_position - position);
  vec3 light_vector = normalize(u_light_position - position);

  float diffuse = max(dot(normal, light_vector), 0.1) * diffuse_intensity;
  diffuse = clamp(diffuse, 0.0, 9.0);
  diffuse = diffuse * (1.0 / ((0.7 * distance)));

  gl_FragColor = vec4((diffuse*diffuse_color) + ambient, 1.0);
}
