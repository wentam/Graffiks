uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;

uniform vec4 u_ambient_color;
uniform vec4 u_diffuse_color;
uniform vec4 u_specular_color;

uniform float u_diffuse_intensity;
uniform float u_specular_intensity;

attribute vec4 a_position;
attribute vec3 a_normal;

varying vec4 v_ambient_color;
varying vec4 v_diffuse_color;
varying vec4 v_specular_color;

varying float v_diffuse_intensity;
varying float v_specular_intensity;

varying vec3 v_position;
varying vec3 v_normal;

void main() {
  v_ambient_color = u_ambient_color;
  v_diffuse_color = u_diffuse_color;
  v_specular_color = u_specular_color;

  v_diffuse_intensity = u_diffuse_intensity;
  v_specular_intensity = u_specular_intensity;

  v_position = vec3((uMVMatrix) * a_position);
  v_normal = vec3(uMVMatrix * vec4(a_normal, 0.0));
  
  gl_Position = (uMVPMatrix) * a_position;
}
