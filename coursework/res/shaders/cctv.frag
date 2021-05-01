#version 430 core


// Our colour filter - calculates colour intensity
const vec3 intensity = vec3(0.299, 0.587, 0.184);
// Captured render
uniform sampler2D tex;
// Alpha map
uniform sampler2D alpha_map;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample textures
  vec4 renderText = texture(tex,tex_coord);
  vec4 alphaText = texture(alpha_map,tex_coord);
  // Final colour is produce of these two colours
  //colour = renderText * alphaText;

  // Calculate grey value
  float grey_value = dot(intensity,vec3(renderText * alphaText));

  //Use greyscale of final colour
  colour = vec4(grey_value, grey_value, grey_value, 1.0);

  //Add sepia
  colour += vec4(0.079,0.043,-0.024,1.0);

  // Ensure alpha is 1
  colour.a = 1.0;
  // *********************************
}