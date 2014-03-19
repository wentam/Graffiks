attribute vec4 a_position;
attribute vec3 a_normal;

uniform mat4 u_mvp_matrix;
uniform mat4 u_mv_matrix;
uniform vec4 u_ambient_color;
uniform vec4 u_diffuse_color;
uniform float u_diffuse_intensity;


varying vec4 v_ambient_color;
varying vec4 v_diffuse_color;
varying float v_diffuse_intensity;
varying vec3 v_position;
varying vec3 v_normal;

void main() {
  v_ambient_color = u_ambient_color;
  v_diffuse_color = u_diffuse_color;

  v_diffuse_intensity = u_diffuse_intensity;

  v_position = vec3((u_mv_matrix) * a_position);
  v_normal = vec3(u_mv_matrix * vec4(a_normal, 0.0));
  
  gl_Position = (u_mvp_matrix) * a_position;
  /*gl_Position = a_position;*/
}
