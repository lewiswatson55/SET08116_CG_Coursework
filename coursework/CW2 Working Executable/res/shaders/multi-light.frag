#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// A directional light structure - N
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light for the scene N
uniform directional_light light;

// Point lights being used in the scene
uniform point_light points[8];
// Spot lights being used in the scene
uniform spot_light spots[2];
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
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
	
	vec3 halfV = normalize(view_dir + light_dir);
	float dotS = dot(halfV, normal);
	float kSpec = max(dotS, 0);
	vec4 specular = mat.specular_reflection * lightCol * pow(kSpec, mat.shininess);


    vec4 primary = mat.emissive + diffuse;

    vec4 colour = primary * tex_colour + specular;
	colour.a = 1;

  // *********************************
  return colour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 L = normalize( spot.position - position);
  vec3 R = spot.direction;

  // Calculate distance to light
  float d = distance(spot.position, position);

  // Calculate attenuation value
  float att = spot.constant + spot.linear*d + spot.quadratic*d*d;

  // Calculate spot light intensity
  float intensity = max(dot(-R, L),0);
  intensity = pow(intensity, spot.power);

  // Calculate light colour
  vec4 lightCol = (intensity/att)*spot.light_colour;

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
	float dotD = dot(normal, L);
	float k = max(dotD, 0);
	vec4 diffuse = mat.diffuse_reflection * lightCol * k;
	
	vec3 halfV = normalize(view_dir + L);
	float dotS = dot(halfV, normal);
	float kSpec = max(dotS, 0);
	vec4 specular = mat.specular_reflection * lightCol * pow(kSpec, mat.shininess);


    vec4 primary = mat.emissive + diffuse;

    vec4 colour = primary * tex_colour + specular;

	colour.a = 1;

  // *********************************
  return colour;
}

vec4 calculate_amb(in directional_light light, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour){
  // Calculate ambient component
  vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;

  // Calculate diffuse component D
  float k = max(dot(normal, light.light_dir),0);
  vec4 diffuse = mat.diffuse_reflection * light.light_colour * k;

  // Calculate view direction
  //vec3 view_dir = normalize(eye_pos - position);

  // Calculate half vector
  vec3 halfV = normalize(view_dir + light.light_dir);

  // Calculate specular component D
  float kSpec = max(dot(halfV, normal), 0);
  vec4 specular = mat.specular_reflection * light.light_colour * pow(kSpec, mat.shininess);

  // Sample texture !t2D
  //vec4 tex_colour = texture(tex, tex_coord);

  // Calculate primary colour component
  vec4 primary = mat.emissive + ambient + diffuse;

  // Calculate final colour - remember alpha
  colour = primary*tex_colour + specular;
  colour.a = 1.0f;

  return colour;
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);

  calculate_amb(light, mat, position, normal, view_dir, tex_colour);

  // Sum point lights
  for (int i = 0; i < 8; ++i) {
		  colour += calculate_point(points[i], mat, position, normal, view_dir, tex_colour);
		  }

  // Sum spot lights
	for (int i = 0; i < 2; ++i) {
		colour += calculate_spot(spots[i], mat, position, normal, view_dir, tex_colour);
		}
	
  // *********************************
}