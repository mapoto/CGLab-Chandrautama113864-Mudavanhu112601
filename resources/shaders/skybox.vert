#version 150
#extension GL_ARB_explicit_attrib_location : require
//gets called for every vertex and computes where this vertex is supposed to be
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;

out vec3 tex_coords;

void main(){
    //invert view matrix to get the last row as last column, so we can access it 
    vec3 camera_pos = inverse(ViewMatrix)[3].xyz;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);

    tex_coords = in_Position;

	
}
