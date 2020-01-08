#version 150

out vec4 out_Color;

in vec3 pass_Normal;
in vec3 frag_Pos;
in vec3 view_Pos;

uniform vec3 planet_Color;
uniform vec3 light_Color;
uniform vec3 light_Pos;
uniform float light_Intensity;

void main() {
  float ambient_Str = 0.1;
  vec3 normal = normalize(pass_Normal);
  
  vec3 dir_View = normalize(view_Pos - frag_Pos);
  vec3 dir_Light = normalize(light_Pos - frag_Pos);
  
  vec3 halfway = normalize(dir_View + dir_Light);

  vec3 ambient = ambient_Str * light_Color* planet_Color;
  
  float diffuse_factor = max(dot(normal,dir_Light),0); 
  vec3 diffuse = diffuse_factor * planet_Color;

  float specular_factor = pow(max(dot(dir_View, halfway), 0.0), 24.0);
  vec3 specular = light_Color* specular_factor;

  out_Color = vec4(ambient + diffuse + light_Intensity*light_Color  + specular, 1.0);

}
