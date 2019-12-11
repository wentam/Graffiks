all: build shaders

build:
	gcc -g -o vl vl.c -lvulkan -lSDL2 -lgraffiks

shaders:
	glslc fragment_shader.frag -o frag.spv
	glslc vertex_shader.vert -o vert.spv

