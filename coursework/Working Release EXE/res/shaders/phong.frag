#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
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
  // Calculate ambient component
  vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;

  // Calculate diffuse component D
  float k = max(dot(normal, light.light_dir),0);
  vec4 diffuse = mat.diffuse_reflection * light.light_colour * k;

  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);

  // Calculate half vector
  vec3 halfV = normalize(view_dir + light.light_dir);

  // Calculate specular component D
  float kSpec = max(dot(halfV, normal), 0);
  vec4 specular = mat.specular_reflection * light.light_colour * pow(kSpec, mat.shininess);

  // Sample texture !t2D
  vec4 tex_colour = texture(tex, tex_coord);

  // Calculate primary colour component
  vec4 primary = mat.emissive + ambient + diffuse;

  // Calculate final colour - remember alpha
  colour = primary*tex_colour + specular;
  colour.a = 1.0f;

  // *********************************
}