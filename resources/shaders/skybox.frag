#version 150

in vec3 tex_coords;

uniform samplerCube SkyTexture;

out vec4 out_color;

void main(){

    out_color = texture(SkyTexture, tex_coords);
    
}