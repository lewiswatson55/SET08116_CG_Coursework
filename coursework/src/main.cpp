#include <glm\glm.hpp>
#include <graphics_framework.h>

// Types of fog
#define FOG_LINEAR 0
#define FOG_EXP 1
#define FOG_EXP2 2

using namespace std;
using namespace graphics_framework;
using namespace glm;

//Maps
map<string, mesh> meshes;
map<string, texture> textures;
map<string, material> materials;

//Light Inits
vector<point_light> points(8);
vector<spot_light> spots(2);
directional_light light;

//Shader Inits
texture tex;
effect leff;
texture normal_map;

vec4 fog_colour = vec4(0.5f, 0.5f, 0.5f, 1.0f);

//Shadows
effect shadow_eff;
shadow_map shadow;

//Skybox Init
effect sky_eff;
mesh skybox;
cubemap cube_map;

effect terrain_eff;
texture terx[4];

//CCTV EFFECT
effect static_eff;
texture alpha_map, static_alpha_map;
void update_static();
int staticframe = 0;
map<int, texture> staticframes;

//Cam Inits
GLFWwindow* window;
target_camera tcam;
target_camera ccam;
free_camera cam;

int current_cam = 0;
double cursor_x = 0.0;
double cursor_y = 0.0;

//Inits Hover Effect 
float hover = 0.0f;
float total_time = 0.0f;

//Motion Blur
frame_buffer frames[2];
frame_buffer temp_frame;
unsigned int current_frame = 0;
geometry screen_quad;
effect motion_blur, tex_eff;

bool initialise() {

    // *********************************
    // Set input mode - hide the cursor
    window = renderer::get_window();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // initialise params

    // Capture initial mouse position
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    // *********************************
    return true;
}

void generate_terrain(geometry& geom, const texture& height_map, unsigned int width, unsigned int depth,
    float height_scale) {
    // Contains our position data
    vector<vec3> positions;
    // Contains our normal data
    vector<vec3> normals;
    // Contains our texture coordinate data
    vector<vec2> tex_coords;
    // Contains our texture weights
    vector<vec4> tex_weights;
    // Contains our index data
    vector<unsigned int> indices;

    // Extract the texture data from the image
    glBindTexture(GL_TEXTURE_2D, height_map.get_id());
    auto data = new vec4[height_map.get_width() * height_map.get_height()];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (void*)data);

    // Determine ratio of height map to geometry
    float width_point = static_cast<float>(width) / static_cast<float>(height_map.get_width());
    float depth_point = static_cast<float>(depth) / static_cast<float>(height_map.get_height());

    // Point to work on
    vec3 point;

    // Part 1 - Iterate through each point, calculate vertex and add to vector
    for (int x = 0; x < height_map.get_width(); ++x) {
        // Calculate x position of point
        point.x = -(width / 2.0f) + (width_point * static_cast<float>(x));

        for (int z = 0; z < height_map.get_height(); ++z) {
            // *********************************
            // Calculate z position of point
            point.z = -(depth / 2.0f) + (depth_point * static_cast<float>(z));
            // *********************************
            // Y position based on red component of height map data
            point.y = data[(z * height_map.get_width()) + x].y * height_scale;
            // Add point to position data
            positions.push_back(point);
        }
    }

    // Part 1 - Add index data
    for (unsigned int x = 0; x < height_map.get_width() - 1; ++x) {
        for (unsigned int y = 0; y < height_map.get_height() - 1; ++y) {
            // Get four corners of patch
            unsigned int top_left = (y * height_map.get_width()) + x;
            unsigned int top_right = (y * height_map.get_width()) + x + 1;
            // *********************************
            unsigned int bottom_left = ((y + 1) * height_map.get_width()) + x;
            unsigned int bottom_right = ((y + 1) * height_map.get_width()) + x + 1;
            // *********************************
            // Push back indices for triangle 1 (tl,br,bl)
            indices.push_back(top_left);
            indices.push_back(bottom_right);
            indices.push_back(bottom_left);
            // Push back indices for triangle 2 (tl,tr,br)
            // *********************************
            indices.push_back(top_left);
            indices.push_back(top_right);
            indices.push_back(bottom_right);
            // *********************************
        }
    }

    // Resize the normals buffer
    normals.resize(positions.size());

    // Part 2 - Calculate normals for the height map
    for (unsigned int i = 0; i < indices.size() / 3; ++i) {
        // Get indices for the triangle
        auto idx1 = indices[i * 3];
        auto idx2 = indices[i * 3 + 1];
        auto idx3 = indices[i * 3 + 2];

        // Calculate two sides of the triangle
        vec3 side1 = positions[idx1] - positions[idx3];
        vec3 side2 = positions[idx1] - positions[idx2];

        // Normal is normal(cross product) of these two sides
        // *********************************
        vec3 n = cross(side2, side1);

        // Add to normals in the normal buffer using the indices for the triangle
        normals[idx1] = normals[idx1] + n;
        normals[idx2] = normals[idx2] + n;
        normals[idx3] = normals[idx3] + n;
        // *********************************
    }

    // Normalize all the normals
    for (auto& n : normals) {
        // *********************************
        n = normalize(n);
        // *********************************
    }

    // Part 3 - Add texture coordinates for geometry
    for (unsigned int x = 0; x < height_map.get_width(); ++x) {
        for (unsigned int z = 0; z < height_map.get_height(); ++z) {
            tex_coords.push_back(vec2(width_point * x, depth_point * z));
        }
    }

    // Part 4 - Calculate texture weights for each vertex
    for (unsigned int x = 0; x < height_map.get_width(); ++x) {
        for (unsigned int z = 0; z < height_map.get_height(); ++z) {
            // Calculate tex weight
            vec4 tex_weight(clamp(1.0f - abs(data[(height_map.get_width() * z) + x].y - 0.0f) / 0.25f, 0.0f, 1.0f),
                clamp(1.0f - abs(data[(height_map.get_width() * z) + x].y - 0.15f) / 0.25f, 0.0f, 1.0f),
                clamp(1.0f - abs(data[(height_map.get_width() * z) + x].y - 0.5f) / 0.25f, 0.0f, 1.0f),
                clamp(1.0f - abs(data[(height_map.get_width() * z) + x].y - 0.9f) / 0.25f, 0.0f, 1.0f));

            // *********************************
            // Sum the components of the vector
            float total = tex_weight.x + tex_weight.y + tex_weight.z + tex_weight.w;
            // Divide weight by sum
            tex_weight = tex_weight / total;
            // Add tex weight to weights
            tex_weights.push_back(tex_weight);
            // *********************************
        }
    }

    // Add necessary buffers to the geometry
    geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
    geom.add_buffer(normals, BUFFER_INDEXES::NORMAL_BUFFER);
    geom.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
    geom.add_buffer(tex_weights, BUFFER_INDEXES::TEXTURE_COORDS_1);
    geom.add_index_buffer(indices);

    // Delete data
    delete[] data;
}


bool load_content() {

    //Terrain
        geometry geom;

        // Load height map
        texture height_map("./res/textures/heightmap.png");

        // Generate terrain
        generate_terrain(geom, height_map, 400, 400, 15.0f);

        // Use geometry to create terrain mesh
        meshes["terr"] = mesh(geom);
        meshes["terr"].get_transform().translate(vec3(0.0f, -10.0f, 0.0f));

        meshes["terr"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
        meshes["terr"].get_material().set_specular(vec4(0.0f, 0.0f, 0.0f, 0.0f));
        meshes["terr"].get_material().set_shininess(20.0f);
        meshes["terr"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));

        textures["terr"] = texture("res/textures/grass2.jpg", true, true);

    // *********Motion Blur**************
    // Create 2 frame buffers - use screen width and height
        frames[0] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
        frames[1] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

        // Create a temp framebuffer
        temp_frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

        // Create screen quad
        vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
            vec3(1.0f, 1.0f, 0.0f) };
        vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
        screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
        screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
        screen_quad.set_type(GL_TRIANGLE_STRIP);
     // *********************************

    //Load Static Frames
      for (int sfs = 1; sfs <= 13; sfs++) {
         staticframes[sfs] = texture("./res/textures/static_frames/static" + to_string(sfs) + ".gif");
      }

    // Load brick_normalmap.jpg texture
    //normal_map = texture("./res/textures/old_normal_map.jpg");

    // Create shadow map
    shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

   //Load Default Texture
   textures["default"] = texture("./res/textures/check_1.png");

  //Create Skybox Mesh, then 
  skybox = mesh(geometry_builder::create_box(vec3(4.5f, 3.0f, 4.0f)));
  skybox.get_transform().scale = vec3(100.0f, 100.0f, 100.0f);
  //skybox.get_transform().translate(vec3(0.0f,100000000.0f,0.0f));

  //Load Skybox Assets 
  array<string, 6> filenames = { "./res/textures/lewisskymap_ft.jpg", "./res/textures/lewisskymap_bk.jpg", "./res/textures/lewisskymap_up.jpg",
                              "./res/textures/lewisskymap_dn.jpg", "./res/textures/lewisskymap_rt.jpg", "./res/textures/lewisskymap_lf.jpg" };

  //array<string, 6> filenames = { "res/textures/sahara_ft.jpg", "res/textures/sahara_bk.jpg", "res/textures/sahara_up.jpg",
    //  "res/textures/sahara_dn.jpg", "res/textures/sahara_rt.jpg", "res/textures/sahara_lf.jpg"};

  cube_map = cubemap(filenames);


  //First Lantern Ball - Optimised
  meshes["LanternBall1"] = mesh(geometry_builder::create_sphere(20,20));
  meshes["LanternBall1"].get_transform().translate(vec3(10.0f, 6.5f, -12.0f));

  //Set Ball Material
  materials["LanternBall"].set_diffuse(vec4(1.0, 1.0, 1.0, 1.0));
  materials["LanternBall"].set_emissive(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  materials["LanternBall"].set_shininess(20.0f);
  materials["LanternBall"].set_specular(vec4(1.0, 1.0, 1.0, 1.0));
  meshes["LanternBall1"].set_material(materials["LanternBall"]);

  //Rest of Lanterns
  meshes["LanternBall2"] = meshes["LanternBall1"];
  meshes["LanternBall2"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  meshes["LanternBall3"] = meshes["LanternBall2"];
  meshes["LanternBall3"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  meshes["LanternBall4"] = meshes["LanternBall1"];
  meshes["LanternBall4"].get_transform().translate(vec3(-20.0f, 0.0f, 0.0f));
  meshes["LanternBall5"] = meshes["LanternBall4"];
  meshes["LanternBall5"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  meshes["LanternBall6"] = meshes["LanternBall5"];
  meshes["LanternBall6"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));

  // Create path mesh
  vec3 pathDimensions(4.0f, 1.0f, 20.0f);
  vec3 sizeBox(6.0f, 4.5f, 6.0f);
  meshes["path"] = mesh(geometry_builder::create_box(pathDimensions));
  meshes["path"].get_transform().scale = sizeBox;
  meshes["path"].get_transform().translate(vec3(0.0f, -2.0f, 40.0f));

  //Set path Material
  materials["path"].set_diffuse(vec4(1.0, 1.0, 1.0, 0.0));
  materials["path"].set_emissive(vec4(0.0, 0.0, 0.0, 0.0));
  materials["path"].set_shininess(5);
  materials["path"].set_specular(vec4(0.2, 0.2, 0.2, 0.0));
  meshes["path"].set_material(materials["path"]);

  //Path Texture
  textures["path"] = texture("res/textures/stone.jpg", true, false);

  // Create platform (for temple) mesh
  vec3 dimensions(5.0f, 3.1f, 5.0f);
  vec3 sizePlatform(12.0f, 2.0f, 12.0f);
  meshes["platform"] = mesh(geometry_builder::create_box(dimensions));
  meshes["platform"].get_transform().scale = sizePlatform;
  meshes["platform"].get_transform().translate(vec3(0.0f, -3.0f, -50.0f));

  //Path Texture
  textures["platform"] = textures["path"];

  //Load Temple - CC License @ https://www.thingiverse.com/thing:4684934
  meshes["temple"] = mesh(geometry("./res/models/TempleOptimised.obj"));
  meshes["temple"].get_transform().translate(vec3(-7.0f, 0.0f, -50.0f)); 

  materials["temple"].set_diffuse(vec4(0.5f, 0.5f, 0.5f, 0.0f));
  materials["temple"].set_emissive(vec4(0.005f, 0.0f, 0.0f, 0.0f));
  materials["temple"].set_shininess(2);
  materials["temple"].set_specular(vec4(0.1f, 0.1f, 0.1f, 1.0f));
  meshes["temple"].set_material(materials["temple"]);


  //Load Cultist - CC License @  https://www.thingiverse.com/thing:1355690
  meshes["cultist"] = mesh(geometry("./res/models/CultistOptimised.obj"));

  //Cultist Texture
  textures["cultist"] = texture("res/textures/cloth.jpg");

  //Set path Material
  materials["cultist"].set_diffuse(vec4(1.0f, 1.0f, 1.0f, 0.0f));
  materials["cultist"].set_emissive(vec4(0.0f, 0.0f, 0.0f, 0.0f));
  materials["cultist"].set_shininess(20);
  materials["cultist"].set_specular(vec4(0.5f, 0.0f, 0.0f, 0.0f));
  meshes["cultist"].set_material(materials["cultist"]);

  //Load Cultist's Balls and transform (position) in Scene
  meshes["cultist_fire"] = mesh(geometry_builder::create_sphere(20,20));
  meshes["cultist_fire"].get_transform().translate(vec3(5.0f, 13.0f, 4.0f));
  textures["cultist_fire"] = texture("res/textures/magic.jpg"); //Set Texture

  //Set Balls Material - Fire
  materials["cultist_fire"].set_diffuse(vec4(1.0, 1.0, 1.0, 1.0));
  materials["cultist_fire"].set_emissive(vec4(1.0, 0.0, 0.0, 1.0));
  materials["cultist_fire"].set_shininess(10.0f);
  materials["cultist_fire"].set_specular(vec4(5.0, 0.0, 0.0, 0.0));
  meshes["cultist_fire"].set_material(materials["cultist_fire"]);

  //Load Water/Blue Ball
  meshes["cultist_water"] = meshes["cultist_fire"];
  meshes["cultist_water"].get_transform().translate(vec3(-10.0f, 1.5f, 0.0f));
  textures["cultist_water"] = textures["cultist_fire"]; //Set Texture

  //Blue Ball Material
  materials["cultist_water"] = materials["cultist_fire"];
  materials["cultist_water"].set_emissive(vec4(1.0, 0.0, 3.0, 1.0));
  materials["cultist_water"].set_specular(vec4(0.0, 0.0, 5.0, 0.0));
  meshes["cultist_water"].set_material(materials["cultist_water"]);

  //Load Lanterns, clone and position relative to previous - Model created by Me (Lewis Watson) in Autodesk Fusion 360
  meshes["right_lantern1"] = mesh(geometry("./res/models/Lamp.obj"));
  vec3 sizeLantern(0.02f, 0.02f, 0.02f);
  meshes["right_lantern1"].get_transform().scale = sizeLantern;
  meshes["right_lantern1"].get_transform().translate(vec3(5.0f, 0.0f, -10.0f));
  textures["right_lantern1"] = texture("res/textures/gran.jpg", true, false); //Set Lantern Texture

  //Lantern Material Set Up
  materials["lantern"].set_diffuse(vec4(1.0, 1.0, 1.0, 1.0));
  materials["lantern"].set_emissive(vec4(0.1f, 0.1f, 0.1f, 1.0f));
  materials["lantern"].set_shininess(3.0f);
  materials["lantern"].set_specular(vec4(1.0, 1.0, 1.0, 1.0));

  //Set Lanterns to Material
  meshes["right_lantern1"].set_material(materials["lantern"]);
  meshes["right_lantern2"].set_material(materials["lantern"]);
  meshes["right_lantern3"].set_material(materials["lantern"]);
  meshes["left_lantern1"].set_material(materials["lantern"]);
  meshes["left_lantern2"].set_material(materials["lantern"]);
  meshes["left_lantern3"].set_material(materials["lantern"]);

  //Continue Loading Lanterns adding the texture
  meshes["right_lantern2"] = meshes["right_lantern1"];
  meshes["right_lantern2"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  textures["right_lantern2"] = textures["right_lantern1"]; //Set Lantern Texture

  meshes["right_lantern3"] = meshes["right_lantern2"];
  meshes["right_lantern3"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  textures["right_lantern3"] = textures["right_lantern1"]; //Set Lantern Texture

  meshes["left_lantern1"] = meshes["right_lantern1"];
  meshes["left_lantern1"].get_transform().translate(vec3(-20.0f, 0.0f, 0.0f));
  textures["left_lantern1"] = textures["right_lantern1"]; //Set Lantern Texture

  meshes["left_lantern2"] = meshes["left_lantern1"];
  meshes["left_lantern2"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  textures["left_lantern2"] = textures["right_lantern1"]; //Set Lantern Texture

  meshes["left_lantern3"] = meshes["left_lantern2"];
  meshes["left_lantern3"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));
  textures["left_lantern3"] = textures["right_lantern1"]; //Set Lantern Texture

  //Point Lights Position
  points[0].set_position(vec3(25.0f, 5.0f, -35.0f));
  points[1].set_position(vec3(-25.0f, 5.0f, -35.0f));

  //Lantern Lights
  points[2].set_position(vec3(10.0f, 6.5f, -12.0f));
  points[3].set_position(vec3(10.0f, 6.5f, 28.0f));
  points[4].set_position(vec3(10.0f, 6.5f, 68.0f));
  points[5].set_position(vec3(-10.0f, 6.5f, -12.0f));
  points[6].set_position(vec3(-10.0f, 6.5f, 28.0f));
  points[7].set_position(vec3(-10.0f, 6.5f, 68.0f));

  //Point Lights Red Properties
  points[0].set_range(50);
  points[0].set_light_colour(vec4(0.3f, 0.0f, 0.0f, 0.0f));
  points[1].set_range(50);
  points[1].set_light_colour(vec4(0.3f, 0.0f, 0.0f, 0.0f));

  //Spot Light Properties
  spots[0].set_position(vec3(2.0f, 10, -25));
  spots[0].set_direction(normalize(vec3(-1, 1, 1)));
  //spots[1].set_position(vec3(-2.0f, 10, -25));
  spots[1].set_position(vec3(45.0f, -20.0f, -45.0f));
  spots[1].set_direction(normalize(vec3(1.0, -2.0, -1.0)));

  //Set all spot lights to same range/colour/power value
  for (auto eb : spots)
  {
      eb.set_range(10);
      eb.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
      eb.set_power(1);
  }

  //Directional Lighting + Ambient
  light.set_ambient_intensity(vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 0.0f));
  light.set_direction(vec3(0, 0, 0));

  // Set the clear colour to be a light grey, the same as our fog. TK
  renderer::setClearColour(0.5f, 0.5f, 0.5f);

  // Shaders, multi frag shader variable and associated vert 
  leff.add_shader("./res/shaders/shader.vert", GL_VERTEX_SHADER);
  vector<string> frag_shaders{ "./res/shaders/shader.frag", "./res/shaders/part_direction.frag",
                            "./res/shaders/part_point.frag", "./res/shaders/part_spot.frag", "./res/shaders/part_shadow.frag", "./res/shaders/part_fog.frag"};
  leff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);


  // Build lights effect
  leff.build(); //Spot and Point Lighting

  //Load and Build Skymap Shader
  sky_eff.add_shader("./res/shaders/skybox.vert", GL_VERTEX_SHADER);
  sky_eff.add_shader("./res/shaders/skybox.frag", GL_FRAGMENT_SHADER);
  sky_eff.build();

  //Shadows
  shadow_eff.add_shader("./res/shaders/point.vert", GL_VERTEX_SHADER);
  shadow_eff.add_shader("./res/shaders/point.frag", GL_FRAGMENT_SHADER);
  shadow_eff.build();

  //Motion Blur
  motion_blur.add_shader("./res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
  motion_blur.add_shader("./res/shaders/motion_blur.frag", GL_FRAGMENT_SHADER);
  motion_blur.build();

  tex_eff.add_shader("./res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
  tex_eff.add_shader("./res/shaders/simple_texture.frag", GL_FRAGMENT_SHADER);
  tex_eff.build();

  //CCTV effect
  static_eff.add_shader("./res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
  static_eff.add_shader("./res/shaders/cctv.frag", GL_FRAGMENT_SHADER);
  static_eff.build();

  // Set camera properties
  cam.set_position(vec3(0.0f, 10.0f, 110.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

//CCTV Static Effect Update - Cycle through frames of animation
void update_static() {
    staticframe++;
    static_alpha_map = staticframes[staticframe];
    if (staticframe == 13) { staticframe = 0; }
}

bool update(float delta_time) {

   // if (glfwGetKey(renderer::get_window(), 'T') == GLFW_PRESS)
        //spots[1].set_position(vec3(10.0f, 0.0f, 0.0f) + spots[1].get_position());
      //  spots[1].set_direction(normalize(vec3(1, 0, 0) + spots[1].get_direction()));

   // if (glfwGetKey(renderer::get_window(), 'Y') == GLFW_PRESS)
        //spots[1].set_position(vec3(0.0f, 10.0f, 0.0f) + spots[1].get_position());
      //  spots[1].set_direction(normalize(vec3(0, 1, 0) + spots[1].get_direction()));

    ///if (glfwGetKey(renderer::get_window(), 'U') == GLFW_PRESS)
        //spots[1].set_position(vec3(0.0f, 0.0f, 10.0f) + spots[1].get_position());
        //spots[1].set_direction(normalize(vec3(0, 0, 1) + spots[1].get_direction()));

    // Flip frame
    current_frame = (current_frame + 1) % 2;

    // Press s to save
    if (glfwGetKey(renderer::get_window(), 'X') == GLFW_PRESS)
        shadow.buffer->save("C:\\Users\\lewis\\OneDrive - Edinburgh Napier University\\Modules\\Second Year\\2 CG\\testt.png");

    //SHADOWS
    // Update the shadow map light_position from the spot light
    shadow.light_position = spots[1].get_position();
    // do the same for light_dir property
    shadow.light_dir = spots[1].get_direction();

    //Move Cultists Balls - Hover Variable
    total_time += delta_time;
    float hover1 = 0.02f * sinf(total_time);
    float hover2 = 0.02f * cosf(total_time);

    //Move Cultists Balls - Transform Balls
    meshes["cultist_fire"].get_transform().translate(vec3(0.0f, hover1, 0.0f));
    meshes["cultist_water"].get_transform().translate(vec3(0.0f, hover2, 0.0f));

    //Get Camera Change Input
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
        current_cam = 1; //Target Cam
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
        current_cam = 2; //Cultist Target Cam
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
        current_cam = 0; //Free Cam


    //Check if current cam is 0
    if (current_cam == 0){

    // The ratio of pixels to rotation - remember the fov
    static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
    static double ratio_height =
        (quarter_pi<float>() *
            (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
        static_cast<float>(renderer::get_screen_height());

    double current_x;
    double current_y;
    // *********************************
    // Get the current cursor position
    glfwGetCursorPos(window, &current_x, &current_y);
    // Calculate delta of cursor positions from last frame

    double delta_x = current_x - cursor_x;
    double delta_y = current_y - cursor_y;

    // Multiply deltas by ratios - gets actual change in orientation
    delta_x *= ratio_width;
    delta_y *= -ratio_height;

    // Rotate cameras by delta
    // delta_y - x-axis rotation
    // delta_x - y-axis rotation
    cam.rotate((float)delta_x, (float)delta_y);

    // Use keyboard to move the camera - WSAD Keystrokes plus cam move
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
        cam.move(vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
        cam.move(vec3(-1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
        cam.move(vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
        cam.move(vec3(0.0f, 0.0f, -1.0f));
    }

    // Update the camera
    cam.update(delta_time);

    // Set skybox position to camera position (camera in centre of skybox)
    //skybox.get_transform().position = cam.get_position();
    skybox.get_transform().position = cam.get_position();


    // Update cursor pos
    cursor_x = current_x;
    cursor_y = current_y;
    // *********************************

    }
    else if (current_cam == 1) {
        tcam.set_position(vec3(50.0f, 5.0f, 110.0f));
        tcam.set_target(vec3(0.0f, 30.0f, 0.0f));
        tcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
        tcam.update(delta_time);
        skybox.get_transform().position = tcam.get_position();
        update_static();

    }
    else if (current_cam == 2) {
        ccam.set_position(vec3(0.0f, 4.0f, 60.0f));
        ccam.set_target(vec3(1.0f, 20.0f, 5.0f));
        ccam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
        ccam.update(delta_time);
        skybox.get_transform().position = ccam.get_position();
    }

    cout << 1 / delta_time << endl;
    return true;
}

bool render() {

    //First Pass
        // Set render target to temp frame
        renderer::set_render_target(temp_frame);
        // Clear frame
        renderer::clear();
    //*********


  //SHADOWS
      // *********************************
    // Set render target to shadow map
    renderer::set_render_target(shadow);
    // Clear depth buffer bit
    glClear(GL_DEPTH_BUFFER_BIT);
    // Set face cull mode to front
    glCullFace(GL_FRONT);
    // *********************************

    // We could just use the Camera's projection, 
    // but that has a narrower FoV than the cone of the spot light, so we would get clipping.
    // so we have yo create a new Proj Mat with a field of view of 90.
    mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

    // Bind shader
    renderer::bind(shadow_eff);
    // Render meshes
    for (auto& e : meshes) {
        auto m = e.second;
        // Create MVP matrix
        auto M = m.get_transform().get_transform_matrix();
        // *********************************
        // View matrix taken from shadow map
        auto V = shadow.get_view();
        // *********************************
        auto MVP = LightProjectionMat * V * M;
        // Set MVP matrix uniform
        glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
            1,                                      // Number of values - 1 mat4
            GL_FALSE,                               // Transpose the matrix?
            value_ptr(MVP));                        // Pointer to matrix data
            // Render mesh
        renderer::render(m);
    }
    // *********************************
      // Set render target back to the screen
    renderer::set_render_target(temp_frame);
    // Set face cull mode to back
    glCullFace(GL_BACK);
    // *********************************

    // ********Render Skymap***********

    //Disable required gl
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    //Bind Sky Effect
    renderer::bind(sky_eff);

    // Calculate MVP for the skybox
    auto M = skybox.get_transform().get_transform_matrix();
    auto V = tcam.get_view();
    auto P = tcam.get_projection();
    mat4 MVP = P * V * M;

    if (current_cam == 0) {
        // Calculate MVP for the skybox
        M = skybox.get_transform().get_transform_matrix();
        V = cam.get_view();
        P = cam.get_projection();
        MVP = P * V * M;
    }
    else if (current_cam == 2) {
        // Calculate MVP for the skybox
        M = skybox.get_transform().get_transform_matrix();
        V = ccam.get_view();
        P = ccam.get_projection();
        MVP = P * V * M;
    }

    // Set MVP matrix uniform
    MVP = P * V * scale(mat4(1.0f), vec3(100.0f));
    glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

    // Set cubemap uniform
    renderer::bind(cube_map, 0);
    glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);

    // Render skybox
    renderer::render(skybox);

    // Enable depth test,depth mask,face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    //*********************************

    // Render meshes 
    for (auto &e : meshes) {
        auto m = e.second;

        // Bind effect
        //renderer::bind(eff);
        renderer::bind(leff);
        // Create MVP matrix
        if (current_cam == 0) {
            auto M = m.get_transform().get_transform_matrix();
            auto V = cam.get_view();
            auto P = cam.get_projection();
            auto MVP = P * V * M;

            // Set M matrix uniform  for LEFF
            glUniformMatrix4fv(leff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
            // Set N matrix uniform - remember - 3x3 matrix  for LEFF
            mat3 N = m.get_transform().get_normal_matrix();
            glUniformMatrix3fv(leff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
            // Set eye position- Get this from active camera  for LEFF
            vec3 eyeP = cam.get_position();
            glUniform3f(leff.get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);

            // Set MVP matrix uniform
            //glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
            glUniformMatrix4fv(leff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

            // Create MV matrix
            auto MV = V * M;
            // Set MV matrix uniform
            glUniformMatrix4fv(leff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));

        }
        else if (current_cam == 1) {
            auto M = m.get_transform().get_transform_matrix();
            auto V = tcam.get_view();
            auto P = tcam.get_projection();
            auto MVP = P * V * M;

            // Set MVP matrix uniform
            //glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
            glUniformMatrix4fv(leff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

            // Set M matrix uniform  for LEFF
            glUniformMatrix4fv(leff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
            // Set N matrix uniform - remember - 3x3 matrix  for LEFF
            mat3 N = m.get_transform().get_normal_matrix();
            glUniformMatrix3fv(leff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

            // Create MV matrix
            auto MV = V * M;
            // Set MV matrix uniform
            glUniformMatrix4fv(leff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));

        }
        else if (current_cam == 2) {
            auto M = m.get_transform().get_transform_matrix();
            auto V = ccam.get_view();
            auto P = ccam.get_projection();
            auto MVP = P * V * M;

            // Set M matrix uniform for LEFF
            glUniformMatrix4fv(leff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
            // Set N matrix uniform - remember - 3x3 matrix  for LEFF
            mat3 N = m.get_transform().get_normal_matrix();
            glUniformMatrix3fv(leff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
            // Set eye position- Get this from active camera  for LEFF
            vec3 eyeP = cam.get_position();
            glUniform3f(leff.get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);

            // Create MV matrix
            auto MV = V * M;
            // Set MV matrix uniform
            glUniformMatrix4fv(leff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));

            // Set MVP matrix uniform
            //glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));      
            glUniformMatrix4fv(leff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

        }

        // Bind material
        material mm = m.get_material();
        renderer::bind(mm, "mat");

        // Bind light
        renderer::bind(light, "light");

        // Bind point lights
        renderer::bind(points, "points");

        // Bind point lights
        renderer::bind(spots, "spots");


        // *********************************
        // Bind defined texture to renderer or default if not availible 

        if (textures[e.first].get_id() != 0)
            renderer::bind(textures[e.first], 0);
        else
            renderer::bind(textures["default"], 0);

        // Set the texture value for the shaders here
        glUniform1i(leff.get_uniform_location("tex"), 0);

        // Set eye position- Get this from active camera  for LEFF
        vec3 eyeP = cam.get_position();
        glUniform3f(leff.get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);
        
        // ***********Fog Effect************
        // Set fog colour to the same as the clear colour - Set as global variable
        glUniform4fv(leff.get_uniform_location("fog_colour"), 1, value_ptr(fog_colour));
        // Set fog start:  5.0f
        glUniform1f(leff.get_uniform_location("fog_start"), 500.0f);
        // Set fog end:  100.0f
        glUniform1f(leff.get_uniform_location("fog_end"), 1000.0f);
        // Set fog density: 0.04f
        glUniform1f(leff.get_uniform_location("fog_density"), 0.001f);
        // Set fog type: FOG_EXP2
        glUniform1i(leff.get_uniform_location("fog_type"), 2);

        //SHADOWS
        // viewmatrix from the shadow map
        auto viewMatrix = shadow.get_view();
        // Multiply together with LightProjectionMat
        LightProjectionMat = LightProjectionMat * shadow.get_view() * M;
        // Set uniform
        glUniformMatrix4fv(leff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(LightProjectionMat));
        // Bind shadow map texture - use texture unit 1
        renderer::bind(shadow.buffer->get_depth(), 1);
        // Set the shadow_map uniform
        glUniform1i(leff.get_uniform_location("shadow_map"), 1);

        // *********************************

        // *********************************

        // Render mesh 
        renderer::render(m);
    }

    //*********Second Pass*********
        // Set render target to current frame
        renderer::set_render_target(frames[current_frame]);
        // Clear frame
        renderer::clear();
        // Bind motion blur effect
        renderer::bind(motion_blur);
        // MVP is now the identity matrix
        MVP = mat4(1.0);
        // Set MVP matrix uniform
        glUniformMatrix4fv(motion_blur.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

        // Bind tempframe to TU 0.
        renderer::bind(temp_frame.get_frame(), 0);
        // Bind frames[(current_frame + 1) % 2] to TU 1.
        renderer::bind(frames[(current_frame + 1) % 2].get_frame(), 1);

        // Set tex uniforms
        glUniform1i(motion_blur.get_uniform_location("tex"), 0);
        glUniform1i(motion_blur.get_uniform_location("previous_frame"), 1);

        // Set blend factor (0.2f)
        glUniform1f(motion_blur.get_uniform_location("blend_factor"), 0.2f);

        // Render screen quad
        renderer::render(screen_quad);
    //******************

        // CCTV CAMERA EFFECT
        if (current_cam == 1) {
            update_static();

            // Set render target back to the screen
            renderer::set_render_target();
            // Bind Tex effect
            renderer::bind(static_eff);
            // MVP is now the identity matrix
            MVP = mat4(1);
            // Set MVP matrix uniform
            glUniformMatrix4fv(static_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

            // Bind texture from frame buffer to TU 0
            renderer::bind(frames[1].get_frame(), 0);
            // Set the tex uniform, 0
            glUniform1i(static_eff.get_uniform_location("tex"), 0);
            // Bind alpha texture to TU, 1
            renderer::bind(static_alpha_map, 1);
            // Set the tex uniform, 1
            glUniform1i(static_eff.get_uniform_location("alpha_map"), 1);
            // Render the screen quad
            renderer::render(screen_quad);


        }

         // Screen Pass
         // Set render target back to the screen
        renderer::set_render_target();

        // empty frame
        renderer::bind(tex_eff);

        // Set MVP matrix uniform
        glUniformMatrix4fv(tex_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

        // Bind texture from frame buffer
        renderer::bind(frames[current_frame].get_frame(), 3);

        // Set the uniform
        glUniform1i(tex_eff.get_uniform_location("tex"), 3);
        // Render the screen quad
        renderer::render(screen_quad);

    return true;
}

void main() {
  // Create application
  app application("Graphics Coursework");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_initialise(initialise);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}