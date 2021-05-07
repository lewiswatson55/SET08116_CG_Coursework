#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Get distance between point light and vertex
  float d = distance(point.position, position);

  // Calculate attenuation factor
  float att = point.constant + point.linear*d + point.quadratic*d*d;

  // Calculate light colour
  vec4 lightCol = point.light_colour * (1/att);


  // Calculate light dir
  vec3 light_dir = normalize(point.position - position);

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float dotD = dot(normal, light_dir);
  float k = max(dotD, 0);
  vec4 diffuse = mat.diffuse_reflection * lightCol * k;

  //Calc view direction
  vec3 view_dir = normalize(eye_pos - position);

  //Half vector
  vec3 halfV = normalize(view_dir + light_dir);

  //Specular Component
  float dotS = dot(halfV, normal);
  float kSpec = max(dotS, 0);
  vec4 specular = mat.specular_reflection * lightCol * pow(kSpec, mat.shininess);

  //Sample tex
  vec4 tex_colour = texture(tex, tex_coord);

  //Primary colour
  vec4 primary = mat.emissive + diffuse;

  //colour
  colour = primary * tex_colour + specular;
  colour.a = 1.0f; //Alpha
  // *********************************
}