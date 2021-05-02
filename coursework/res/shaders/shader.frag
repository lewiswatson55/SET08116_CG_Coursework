#version 440

// This shader requires part_direction.frag, part_point.frag,part_spot.frag 

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);
float calculate_fog(in float fog_coord, in vec4 fog_colour, in float fog_start, in float fog_end, in float fog_density, in int fog_type);

// Directional light information
uniform directional_light light;
// Point lights being used in the scene
uniform point_light points[8];
// Spot lights being used in the scene
uniform spot_light spots[2];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Normal map to sample from
uniform sampler2D normal_map;
// Shadow map to sample from
uniform sampler2D shadow_map;
// Fog colour
uniform vec4 fog_colour;
// Fog start position
uniform float fog_start;
// Fog end position
uniform float fog_end;
// Fog density
uniform float fog_density;
// Fog type
uniform int fog_type;


// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming binormal
layout(location = 2) in vec3 binormal;
// Incoming tangent
layout(location = 3) in vec3 tangent;
// Incoming light space position
layout(location = 4) in vec4 light_space_pos;
// Camera space position
layout(location = 5) in vec4 CS_position;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {

  //Shadows
  // Calculate shade factor
  //float shade = calculate_shadow(shadow_map, light_space_pos);

  // *********************************
  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);


   // Calculate directional light colour
  colour += calculate_direction(light, mat, normal, view_dir, tex_colour);

  // Sum point lights
  for (int i = 0; i < 8; ++i) {
		  colour += calculate_point(points[i], mat, position, normal, view_dir, tex_colour);
  }

  // Sum spot lights
	for (int i = 0; i < 2; ++i) {
		colour += calculate_spot(spots[i], mat, position, normal, view_dir, tex_colour);
	}

  // Scale colour by shade
	//colour *= shade;

  // Ensure alpha is 1.0f
  colour.a = 1.0f;

  //Calculate Foggy Bois
	  // Calculate fog coord
	  float fog_coord = abs(CS_position.z / CS_position.w);

	  // Calculate fog factor
	  float fog_factor = calculate_fog(fog_coord, fog_colour, fog_start, fog_end, fog_density, fog_type);

	  // Final colour is mix between colour and fog colour based on factor
	  colour = mix(colour,fog_colour,fog_factor);
	  colour.a = 1.0f;

  // *********************************
}