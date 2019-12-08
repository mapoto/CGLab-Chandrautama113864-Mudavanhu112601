#version 150
#extension GL_ARB_explicit_attrib_location : require

// glVertexAttribPointer mapped positions to first
// vertex attributes of glVertexAttribPointer VAO

layout(location = 0) in vec3 in_Position;

// glVertexAttribPointer mapped color  to second attribute 

layout(location = 1) in vec3 in_Color;

//Matrix Uniforms uploaded with glUniform*
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_Color;

void main() {
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	pass_Color = in_Color;
}