#version 430 core

// Incoming texture containing frame information
uniform sampler2D tex;

// Our colour filter - calculates colour intensity
const vec3 intensity = vec3(0.299, 0.587, 0.184);

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample texture colour
  vec4 sample_texture = texture(tex, tex_coord);

  // Calculate grey value
  float grey_value = dot(intensity,vec3(sample_texture));
  // Use greyscale to as final colour
  // - ensure alpha is 1
  colour = vec4(grey_value, grey_value, grey_value, 1.0);

  //Add Slight Sapia Effect (edited with own values for cwk2)
  // Saved from lab 0.341, 0.169, -0.090
  colour += vec4(0.079,0.043,-0.024,1.0);



  //ensure alpha
  colour.a=1.0;
  // *********************************
}