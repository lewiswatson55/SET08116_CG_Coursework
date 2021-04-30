#include <glm\glm.hpp>
#include <graphics_framework.h>

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

//Shadows
effect shadow_eff;
shadow_map shadow;

//Skybox Init
effect sky_eff;
mesh skybox;
cubemap cube_map;

//Cam Inits
GLFWwindow* window;
target_camera tcam;
target_camera ccam;
free_camera cam;

int current_cam = 1;
double cursor_x = 0.0;
double cursor_y = 0.0;

//Inits Hover Effect 
float hover = 0.0f;
float total_time = 0.0f;

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

bool load_content() {

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


  // Create plane mesh 
  meshes["plane"] = mesh(geometry_builder::create_plane());
  vec3 sizePlane(1.5f, 2.0f, 2.0f);
  meshes["plane"].get_transform().scale = sizePlane;
  textures["plane"] = texture("res/textures/grass.jpg", true, true);

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


  //Set Plane Material - THE GRASS
  materials["plane"].set_diffuse(vec4(1.0, 1.0, 1.0, 1.0));
  materials["plane"].set_emissive(vec4(0.0, 0.0, 0.0, 1.0));
  materials["plane"].set_shininess(0.5f);
  materials["plane"].set_specular(vec4(1.0, 1.0, 1.0, 1.0));
  meshes["plane"].set_material(materials["plane"]);

  // Create path mesh
  vec3 pathDimensions(4.0f, 0.1f, 20.0f);
  vec3 sizeBox(6.0f, 3.0f, 6.0f);
  meshes["path"] = mesh(geometry_builder::create_box(pathDimensions));
  meshes["path"].get_transform().scale = sizeBox;
  meshes["path"].get_transform().translate(vec3(0.0f, 0.0f, 40.0f));

  //Set path Material
  materials["path"].set_diffuse(vec4(1.0, 1.0, 1.0, 0.0));
  materials["path"].set_emissive(vec4(0.0, 0.0, 0.0, 0.0));
  materials["path"].set_shininess(5);
  materials["path"].set_specular(vec4(0.2, 0.2, 0.2, 0.0));
  meshes["path"].set_material(materials["path"]);

  //Path Texture
  textures["path"] = texture("res/textures/stone.jpg", true, false);

  // Create platform (for temple) mesh
  vec3 dimensions(5.0f, 0.2f, 5.0f);
  vec3 sizePlatform(12.0f, 0.2f, 12.0f);
  meshes["platform"] = mesh(geometry_builder::create_box(dimensions));
  meshes["platform"].get_transform().scale = sizePlatform;
  meshes["platform"].get_transform().translate(vec3(0.0f, 0.0f, -50.0f));

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
  spots[1].set_position(vec3(-2.0f, 10, -25));
  spots[1].set_direction(normalize(vec3(-1, 1, 1)));

  //Set all spot lights to same range/colour/power value
  for (auto eb : spots)
  {
      eb.set_range(10);
      eb.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
      eb.set_power(1);
  }

  //Lighting 
  light.set_ambient_intensity(vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 0.0f));
  light.set_direction(vec3(1, 1, -1));


  // Shaders, multi frag shader variable and associated vert
  leff.add_shader("./res/shaders/shader.vert", GL_VERTEX_SHADER);
  vector<string> frag_shaders{ "./res/shaders/shader.frag", "./res/shaders/part_direction.frag",
                            "./res/shaders/part_point.frag", "./res/shaders/part_spot.frag", "./res/shaders/part_shadow.frag" };
  leff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);

  //leff.add_shader("./res/shaders/multi-light.frag", GL_FRAGMENT_SHADER); 
  //leff.add_shader("./res/shaders/multi-light.vert", GL_VERTEX_SHADER);


  // Build lights effect
  leff.build(); //Spot and Point Lighting

  //Load and Build Skymap Shader
  sky_eff.add_shader("./res/shaders/skybox.vert", GL_VERTEX_SHADER);
  sky_eff.add_shader("./res/shaders/skybox.frag", GL_FRAGMENT_SHADER);
  sky_eff.build();


  //Shadows
  shadow_eff.add_shader("./res/shaders/spot.vert", GL_VERTEX_SHADER);
  shadow_eff.add_shader("./res/shaders/spot.frag", GL_FRAGMENT_SHADER);
  shadow_eff.build();


  // Set camera properties
  cam.set_position(vec3(0.0f, 10.0f, 110.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}


bool update(float delta_time) {

    // Press s to save
    if (glfwGetKey(renderer::get_window(), 'x') == GLFW_PRESS)
        shadow.buffer->save("C:\\Users\\lewis\\OneDrive - Edinburgh Napier University\\Modules\\Second Year\\2 CG\\test.png");

    //SHADOWS
    // Update the shadow map light_position from the spot light
    shadow.light_position = spots[0].get_position();
    // do the same for light_dir property
    shadow.light_dir = spots[0].get_direction();

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
    renderer::set_render_target();
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

        // Set the texture value for the shaders heree
        glUniform1i(leff.get_uniform_location("tex"), 0);

        // Set eye position- Get this from active camera  for LEFF 
        vec3 eyeP = cam.get_position();
        glUniform3f(leff.get_uniform_location("eye_pos"), eyeP.x, eyeP.y, eyeP.z);

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
        // Render mesh 
        renderer::render(m);
    }

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