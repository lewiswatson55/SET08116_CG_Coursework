// Types of fog
#ifndef FOG_TYPES
#define FOG_TYPES
#define FOG_LINEAR 0
#define FOG_EXP 1
#define FOG_EXP2 2
#endif

float calculate_fog(in float fog_coord, in vec4 fog_colour, in float fog_start, in float fog_end, in float fog_density,
                    in int fog_type);

// Sampler used to get texture colour
uniform sampler2D tex;
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

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Camera space position
layout(location = 1) in vec4 CS_position;

// Outgoing colour
layout(location = 0) out vec4 out_colour;

float calculate_fog(in float fog_coord, in vec4 fog_colour, in float fog_start, in float fog_end, in float fog_density,
                    in int fog_type) {
  // Fog value to return
  float result = 0.0;

  // Calculate fog
  if (fog_type == FOG_LINEAR) {
    // Linear fog
    result = (fog_end - fog_coord) / (fog_end - fog_start);
  } else if (fog_type == FOG_EXP) {
    // *********************************
    // Exponential fog
	result = exp((-1.0) * fog_density * fog_coord);
    // *********************************
  } else if (fog_type == FOG_EXP2) {
    // *********************************
    // Exponential squared fog
	result = pow(exp((-1.0) * fog_density * fog_coord), 2.0);
    // *********************************
  }
  // *********************************
  // Result is 1 minus result clamped to 1.0 to 0.0
  result =  1.0 - clamp(result, 0.0, 1.0);
  // *********************************
  return result;
}

void main() {
  vec4 colour = vec4(0.0f,0.0f,0.0f,1.0f);
  // *********************************
  // Set out colour to sampled texture colour

  float fog_coord = abs(CS_position.z / CS_position.w);

  // Calculate fog factor
  float fog_factor = calculate_fog(fog_coord, fog_colour, fog_start, fog_end, fog_density, fog_type);


  // Colour is mix between colour and fog colour based on factor
  colour = mix(colour,fog_colour,fog_factor);

  out_colour = texture(tex, tex_coord);
  // *********************************
}