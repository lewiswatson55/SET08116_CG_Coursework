#version 440

// Model transformation matrix
uniform mat4 M;
// Transformation matrix
uniform mat4 MVP;
// Normal matrix
uniform mat3 N;
// The light transformation matrix
uniform mat4 lightMVP;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming binormal
layout(location = 3) in vec3 binormal;
// Incoming tangent
layout(location = 4) in vec3 tangent;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing position
layout (location = 0) out vec3 vertex_position;
// Outgoing transformed normal
layout (location = 1) out vec3 transformed_normal;
// Incomnif binormal
layout(location = 2) out vec3 binormal_out;
// Incoming tangent
layout(location = 3) out vec3 tangent_out;
// Outgoing position in light space
layout (location = 4) out vec4 vertex_light;
// Outgoing texture coordinate
layout (location = 10) out vec2 tex_coord_out;

void main()
{
  // Calculate screen position
  gl_Position = MVP * vec4(position, 1.0);
  // *********************************
  // Output other values to fragment shader

  ///vertex_position = (M * vec4(position, 1.0)).xyz;
  vertex_position = vec3(M * vec4(position, 1.0f));

  // Transform normal
  transformed_normal = N * normal;

  // Pass through tex_coord
  tex_coord_out = tex_coord_in;

  // normal map
  // Transform tangent
  tangent_out = N * tangent;
  // Transform binormal
  binormal_out = N * binormal;

  //SHADOW STUFF
    // Transform position into light space
    vertex_light = lightMVP * vec4(position, 1.0f);

  // *********************************
}