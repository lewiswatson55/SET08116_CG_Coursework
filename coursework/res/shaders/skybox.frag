#version 410

// Cubemap texture
uniform samplerCube cubemap;
// Fog colour
uniform vec4 fog_colour;

// Incoming 3D texture coordinate
layout (location = 0) in vec3 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	// ************************
	// Sample texture as normal
	// ************************
	colour = texture(cubemap, tex_coord);

  // Colour is mix between colour and fog colour based on factor
  colour = mix(colour,fog_colour,0.5f);
  colour.a = 1.0f;
}