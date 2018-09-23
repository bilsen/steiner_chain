#version 330 core
layout (location = 0) in vec2 aPos; // the position variable has attribute position 0

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 0.0, 1.0); // see how we directly give a vec3 to vec4's constructor
	
}