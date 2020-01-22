#version 150

out vec4 out_Color;

in vec3 pass_Normal;
in vec3 frag_Pos;
in vec3 view_Pos;
in vec2 texture_Coord;

uniform vec3 planet_Color;
uniform vec3 light_Color;
uniform vec3 light_Pos;
uniform float light_Intensity;

uniform sampler2D planet_Texture;


void main() {
  float ambient_Str = 0.1;
  vec3 normal = normalize(pass_Normal);
  
  vec3 dir_View = normalize(view_Pos - frag_Pos);
  vec3 dir_Light = normalize(light_Pos - frag_Pos);
  
  vec3 halfway = normalize(dir_View + dir_Light);

  vec4 ambient = vec4(ambient_Str  * planet_Color, 1.0);
  
  float diffuse_factor = max(dot(normal,dir_Light),0); 
  vec4 diffuse = diffuse_factor * texture(planet_Texture,texture_Coord);

  float specular_factor = pow(max(dot(dir_View, halfway), 0.0), 24.0);
  vec4 specular = vec4(light_Color * specular_factor,1.0);

  out_Color = ambient + diffuse + specular;

}
