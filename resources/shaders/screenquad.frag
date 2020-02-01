#version 150

in vec2 tex_coords;

uniform sampler2D FBTexture;
uniform bool ShaderMode_grey;
uniform bool ShaderMode_verticalMirror;
uniform bool ShaderMode_horizontalMirror;
uniform bool ShaderMode_blur;

out vec4 out_color;

void main(){
     
    vec2 texCoords = tex_coords;
    
    //Default outcolor, if nothing else is applied
    out_color = texture(FBTexture, texCoords);

}