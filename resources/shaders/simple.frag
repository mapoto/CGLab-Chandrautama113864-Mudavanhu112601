#version 150

in  vec3 pass_Normal;
out vec4 out_Color;
uniform vec3 planet_col;

uniform vec3 LightColor;
uniform float LightIntensity;


void main() {
  out_Color = vec4(planet_col, 1.0);
  

}
