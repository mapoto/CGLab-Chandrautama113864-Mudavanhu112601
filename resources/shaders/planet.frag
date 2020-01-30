#version 150

out vec4 out_Color;

in vec3 pass_Normal;
in vec3 frag_Pos;
in vec3 view_Pos;
in vec2 texture_Coord;
in mat4 pass_ViewMatrix;

uniform vec3 planet_Color;
uniform vec3 light_Color;
uniform vec3 light_Pos;
uniform float light_Intensity;

uniform sampler2D planet_Texture;

float light_Cons = 1.0f; 
float light_Linear = 0.01f; 
float light_Quad = 0.005f; 

void main() {

/* --------------------------------- ambient -------------------------------- */
  float ambient_Str = 0.1;
  vec3 ambient = ambient_Str  * light_Color;
  vec3 ambient_result = ambient_Str  * light_Color;

/* --------------------------------- diffuse -------------------------------- */
  vec3 normal = normalize(pass_Normal);
  vec3 dir_Light = (pass_ViewMatrix*vec4(light_Pos,1.0)).xyz - frag_Pos;
  vec3 norm_dir_Light = normalize(dir_Light);
  float diffuse = max(dot(norm_dir_Light, normal),0); 
  vec3 diffuse_result =  diffuse  * vec3(texture(planet_Texture,texture_Coord));

/* -------------------------------- specular -------------------------------- */
  vec3 dir_View = normalize(view_Pos - frag_Pos);
  vec3 halfway = normalize(dir_View + norm_dir_Light);

  float specularStrength = 0.2;
  float specular_factor = pow(max(dot(dir_View, halfway), 0.0), 10.0);
  vec3 specular_result = light_Color * specular_factor * vec3(texture(planet_Texture,texture_Coord));

/* ------------------------------- attenuation ------------------------------ */
  float distance = length((pass_ViewMatrix*vec4(light_Pos,1.0)).xyz - frag_Pos);
  float attenuation = 1.0f / (light_Cons + light_Linear * distance + light_Quad * (distance * distance));

  out_Color = vec4(attenuation*(ambient_result + diffuse_result + specular_result), 1.0);
  
}
