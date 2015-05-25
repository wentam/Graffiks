#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec4 in_position;

out vec3 v_position;

void main() {
  v_position = vec3(in_position);

  gl_Position = in_position;
}
