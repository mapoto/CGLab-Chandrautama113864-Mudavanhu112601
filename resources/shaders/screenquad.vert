#version 150
#extension GL_ARB_explicit_attrib_location : require
//gets called for every vertex and computes where this vertex is supposed to be
// vertex attributes of VAO
layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_TexCoords;

out vec2 tex_coords;

void main(){
    //everything is 2D now since it's a plane!
    //so z is just 0
    gl_Position = vec4(in_Position, 0.0, 1.0);
    tex_coords = in_TexCoords;
}