#include "application_solar.hpp"
#include "window_handler.hpp"

#include "model_loader.hpp"
#include "shader_loader.hpp"
#include "utils.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

// dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
/////////////////////////////////////////////////////////////////////////////////////
//User Defined Solar Application Constructor 

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
    : Application{resource_path},
      scene_graph{},
      planet_object{},
      m_view_transform{},
      m_view_projection{
          utils::calculate_projection_matrix(initial_aspect_ratio)} {
  initialize_scene_graph();
  initializeShaderPrograms();
}

////////////////////////////////////////////////////////////////////////////////////
//The Application Destructor used to end the and free resources used in the Application

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

/////////////////////////////////////////////////////////////////////////////////////
//Rendering the Solar System Application

void ApplicationSolar::render() const {
  // list of nodes in graph below the root including sun, camera and planets
  auto const solar_system = scene_graph.getRoot()->getChildrenList();

  // initial distance
  glm::fvec3 distance = glm::fvec3{0.0f, 0.0f, 0.0f};

  // a pivot for planets' revolution = sun's world_matrix
  glm::fmat4 const solar_system_origin =
      scene_graph.getRoot()->getWorldTransform();

  render_scene(solar_system, distance, solar_system_origin);
}

//Rendering all the Nodes in the Scene by looping through the Tree (SceneGraph)
void ApplicationSolar::render_scene(
    std::list<Node*> const& sol,
    glm::fvec3& distance,
    glm::fmat4 const& solar_system_origin) const {
  int planet_rotation_speed_factor = 1;
  // loop through all elements below the root
  for (auto planet : sol) {
    // ignore rendering the camera
    if (planet->getName() != "camera") {
      // calculate the matrix of each planet
      process_planet_matrix(planet, distance, solar_system_origin,
                            planet_rotation_speed_factor);

      // use it to get the image for this frame
      render_node(planet);

      auto moons = planet->getChildrenList();

      if (!moons.empty()) {
        glm::fvec3 moon_distance_from_planet = glm::fvec3{2.0f, 0.0f, 0.0f};
        glm::fvec3 moon_size = glm::fvec3{0.5f};

        for (auto moon : moons) {
          if (moon->getName() == "holder_moon") {
            process_moon_matrix(moon, planet, moon_distance_from_planet,
                                moon_size);
            render_node(moon);
          }
        }
      }

      // lazy increment for the next planet
      distance += glm::fvec3{4.0f, 0.0f, 0.0f};
      ++planet_rotation_speed_factor;
    }
  }
}

// calculate the matrix
void ApplicationSolar::process_planet_matrix(
    Node* planet,
    glm::fvec3& distance,
    glm::fmat4 const& solar_system_origin,
    int speed_factor) const {
  // calculate the planet's matrix when it's translated to the correct
  // distance from the root.
  glm::fmat4 planet_matrix =
      glm::translate(planet->getWorldTransform(), distance);

  // get the rotation with respect to the origin of the solar system
  planet_matrix =
      glm::rotate(solar_system_origin, float(4 * glfwGetTime() / speed_factor),
                  glm::fvec3{0.0f, 1.0f, 0.0f});

  planet->getChildrenList().front()->setWorldTransform(glm::rotate(
      glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f}));

  // allow it move along its orbit to allow planet's revolution around the sun
  planet_matrix = glm::translate(planet_matrix, distance);

  if (planet->getName() == "holder_sun") {
    planet_matrix = glm::scale(planet_matrix, glm::fvec3{1.8f});
  }

  // set the orbit as the planet world transform
  planet->setWorldTransform(planet_matrix);
}

/////////////////////////////////////////////////////////////////////////////
//The Moon Matrix that give the Moon its attributes with relativity to its 
//Parent Planet 

void ApplicationSolar::process_moon_matrix(
    Node* moon,
    Node* planet,
    glm::fvec3 const& distance_from_planet,
    glm::fvec3 const& moon_size) const {
  
  // multiplication to bring it to the same world coordinate system
  glm::fmat4 moon_matrix =
      glm::translate(moon->getWorldTransform() * planet->getWorldTransform(),
                     distance_from_planet);

  // get the rotation with respect to the origin of the solar system
  moon_matrix = glm::rotate(planet->getWorldTransform(), float(glfwGetTime()),
                            glm::fvec3{0.0f, 1.0f, 0.0f});

  // allow it move along its orbit
  moon_matrix = glm::translate(moon_matrix, distance_from_planet);

  moon_matrix = glm::scale(moon_matrix, moon_size);

  moon->setWorldTransform(moon_matrix);
}

//////////////////////////////////////////////////////////////////////////////
//Rendering the Planet using the shaders 

void ApplicationSolar::render_node(Node* planet) const {
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);

  // get the planet matrix at this frame
  glm::fmat4 model_matrix = planet->getWorldTransform();

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1,
                     GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix =
      glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"), 1,
                     GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements,
                 model::INDEX.type, NULL);
}

//////////////////////////////////////////////////////////////////////////////
//Setting the View od the Camera

void ApplicationSolar::set_m_view_transform(glm::fmat4 const& cam_matrix) {
  m_view_transform = cam_matrix;
}

//////////////////////////////////////////////////////////////////////////////
//Updating the new Vview of the Camera 

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be
  // inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"), 1,
                     GL_FALSE, glm::value_ptr(view_matrix));
}

///////////////////////////////////////////////////////////////////////////////
//Uploading the Projection to be processed by the GPU from the Memory

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"), 1,
                     GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to std::make_shared< loca>(ions
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////

// Populate the scene_graph with all the necessary nodes
void ApplicationSolar::initialize_scene_graph() {
  // load the model to be used in geometry nodes creation and shader
  model planet_model;
  initializeGeometry(planet_model);

  // Create root node as a pointer to prevent it being destroyed in stack
  Node* root_node = new Node{"root"};
  scene_graph.setRoot(root_node);
  scene_graph.setName("scene_graph_1");

  // Build the entire graph one nodes at a time (with geometry for the planets)
  create_camera("camera");
  create_sun("holder_sun", planet_model);
  create_planet("holder_mercury", planet_model);
  create_planet("holder_venus", planet_model);
  create_planet("holder_earth", planet_model);
  create_planet("holder_mars", planet_model);
  create_planet("holder_jupiter", planet_model);
  create_planet("holder_saturn", planet_model);
  create_planet("holder_uranus", planet_model);
  create_planet("holder_neptune", planet_model);

  //Craeting the Moon's obit and attaching it to the Earths Orbit 
  create_moon_for_planet("holder_earth", "holder_moon",planet_model);

  //Printing the Scenegraph
  std::cout << scene_graph.printGraph() << std::endl;
}

// Create camera node
void ApplicationSolar::create_camera(std::string const& camera_name) {
  // As a pointer to prevent it being destroyed in stack
  CameraNode* cam = new CameraNode{camera_name};

  // Setter for further modification
  cam->setEnabled(true);
  cam->setPerspective(true);

  // Attach camera node to directly to the node
  scene_graph.getRoot()->addChild(cam);

  // Some in place variable to shorten the code
  auto cam_world_matrix = cam->getWorldTransform();
  auto cam_local_matrix = cam->getLocalTransform();

  // translate camera position upwards
  cam_world_matrix =
      glm::translate(cam_world_matrix, glm::fvec3{0.0f, 50.0f, 0.0F});

  // rotate the camera so it is facing downwards
  cam_world_matrix =
      glm::rotate(cam_world_matrix, 3.14f / 2, glm::vec3{-1.0f, 0.0f, 0.0f});

  // the new transformation to the camera with respect to the world
  cam->setWorldTransform(cam_world_matrix);

  // calculate the model matrix cam
  glm::fmat4 model_matrix_cam = cam_world_matrix * cam_local_matrix;

  // use the result as view transform matrix for the viewport used by
  // application
  set_m_view_transform(model_matrix_cam);
}

// create sun node with sun_name as its name and take loaded model
void ApplicationSolar::create_sun(std::string const& sun_name,
                                  model const& sun_model) {
  // As a normal node pointer until light is fully implemented
  Node* sun_holder = new Node{sun_name};

  // Create its the geometry
  GeometryNode* sun_geometry =
      new GeometryNode{"geometry_" + sun_name, sun_model};

  // Attach the sun node to the root directly
  scene_graph.getRoot()->addChild(sun_holder);

  // Attach its geometry to the sun node
  sun_holder->addChild(sun_geometry);
}

// create a planet node with planet_name and a loaded model for its geometry
void ApplicationSolar::create_planet(std::string const& planet_name,
                                     model const& planet_model) {
  // Create it as node pointer to prevent it being destroyed
  Node* planet = new Node{planet_name};

  // Attach the planet node to the root directly
  scene_graph.getRoot()->addChild(planet);

  // Create its the geometry
  GeometryNode* geometry =
      new GeometryNode{"geometry_" + planet_name, planet_model};

  // Attach its geometry to the planet node
  planet->addChild(geometry);
}

// Create a moon for a planet using its name
void ApplicationSolar::create_moon_for_planet(std::string const& planet_name,
                                              std::string const& moon_name,
                                              model const& moon_model) {
  // find the planet by its name and assign it to a in place variable
  auto wanted_planet = (scene_graph.getRoot())->getChild(planet_name);

  if (wanted_planet != nullptr) {
    // Create it as node pointer to prevent it being destroyed
    Node* moon = new Node{moon_name};
    wanted_planet->addChild(moon);

    // Create its the geometry with the model
    GeometryNode* moon_geometry =
        new GeometryNode{"geometry_" + moon_name, moon_model};

    // add the geometry to the moon
    moon->addChild(moon_geometry);
  }
}

// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace(
      "planet",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/simple.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry(model& planet_model) {
  planet_model =
      model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(),
               planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type,
                        GL_FALSE, planet_model.vertex_bytes,
                        planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type,
                        GL_FALSE, planet_model.vertex_bytes,
                        planet_model.offsets[model::NORMAL]);

  // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               model::INDEX.size * planet_model.indices.size(),
               planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -1.0f});
    uploadView();
    std::cout << "key W pressed: camera Zoom In" << std::endl;
    
  } else if (key == GLFW_KEY_S &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 1.0f});
    uploadView();
    std::cout << "key S pressed: camera Zoom Out" << std::endl;

  } else if (key == GLFW_KEY_A &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
    uploadView();
    std::cout << "key A pressed: Camera Angle Move Left" << std::endl;

  } else if (key == GLFW_KEY_D &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    uploadView();
    std::cout << "key D pressed: camera Angle Move Right" << std::endl;
  }

  ///////////////////////////////////ARROW KEYS FOR CONTROL////////////////////////
  //I added the other keyboard controls to the seen we can now use the arrow keys as well

   else if (key == GLFW_KEY_RIGHT &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    uploadView();
    std::cout << "key Right Arrow pressed: camera Angle Move Right" << std::endl;
  }

     else if (key == GLFW_KEY_LEFT &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
    uploadView();
    std::cout << "key Left Arrow pressed: camera Angle Move Left" << std::endl;
  }

      else if (key == GLFW_KEY_DOWN &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{0.0f, 1.0f, 0.0f});
    uploadView();
    std::cout << "key Down Arrow pressed: camera Zoom Out" << std::endl;
  }

      else if (key == GLFW_KEY_UP &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{0.0f, -1.0f, 0.0f});
    uploadView();
    std::cout << "key Up Arrow pressed: camera Zoom In" << std::endl;
  }

  //////////////////////////SCENE ROTATION ////////////////////////////////////
  //So far l managed to make the scene rotate but its rotating arounf the camera
  //Will see how best l can make it rotate around the sun in stead 

      else if (key == GLFW_KEY_V &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::rotate(m_view_transform, 0.01f, glm::fvec3{0.0f, 1.0f, 0.0f});
    uploadView();
    std::cout << "key V pressed: camera Rotate Clockwise" << std::endl;
  }

     else if (key == GLFW_KEY_X &&
             (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::rotate(m_view_transform, 0.01f, glm::fvec3{0.0f, -1.0f, 0.0f});
    uploadView();
    std::cout << "key X pressed: camera Rotate AntiClockwise" << std::endl;
  }
}

// handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
  std::cout << pos_x << ":" << pos_y << std::endl;
  
  // as a first person camera
  m_view_transform =
      glm::rotate(m_view_transform, 0.005f, glm::vec3{pos_y, pos_x, 0.0f});
  uploadView();
}

// handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for std::make_shared< aspe>(t ration
  m_view_projection =
      utils::calculate_projection_matrix(float(width) / float(height));
  // upload std::make_shared< proj>(ction matrix
  uploadProjection();
}

// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}