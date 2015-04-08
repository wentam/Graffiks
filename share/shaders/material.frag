precision mediump float;

uniform vec3 u_light_position;

varying vec4 v_ambient_color;
varying vec4 v_diffuse_color;
varying float v_diffuse_intensity;
varying vec3 v_position;
varying vec3 v_normal;

void main() {
  float distance = length(u_light_position - v_position);
  vec3 light_vector = normalize(u_light_position - v_position);

  /* calculate diffuse intensity for this fragment */
  float diffuse = max(dot(v_normal, light_vector), 0.1);
  diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));

  /* combine target color with diffuse intensity and ambient color. set as result. */
  gl_FragColor = ((v_diffuse_color) * (diffuse*(v_diffuse_intensity*4.0))) + v_ambient_color;
  /*gl_FragColor = vec4(0.0,1.0,0.0,1.0);*/
}
