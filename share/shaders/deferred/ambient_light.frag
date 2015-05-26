#version 330
#extension GL_ARB_explicit_uniform_location : enable

precision mediump float;

layout(location = 1) uniform vec3 u_ambient_color;

in vec3 v_position;

void main() {
  gl_FragColor = vec4(u_ambient_color, 1.0);
}
