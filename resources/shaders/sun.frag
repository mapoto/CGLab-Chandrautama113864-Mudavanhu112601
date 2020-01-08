#version 150

in  vec3 pass_Normal;
out vec4 out_Color;

uniform vec3 sun_Color;

void main() {
  out_Color = vec4(sun_Color, 1.0);
}