#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "SceneGraph.hpp"
#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "GeometryNode.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  // handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  // handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;
  void renderStars() const; 

 protected:
  /////////////////////////////////////////////////////////////////////////////////////////
  //initializing the SceneGraph, the Shader and the Geometry
  
  void initialize_scene_graph();
  void initializeShaderPrograms();
  void initializeGeometry(model& planet_model);
  void initializeGeometry();
  void initializeStars();

  // update uniform values
  void uploadUniforms();

  // upload projection matrix
  void uploadProjection();

  // upload view matrix
  void uploadView();

  //seeting the View of the Camera
  void set_m_view_transform(glm::fmat4 const& cam_matrix);

 private:
  ////////////////////////////////////////////////////////////////////////////////////
  //Craeting the Camera which we will use for perspective 
  void create_camera(std::string const& camera_name);

  //Creating the Sun, giving it its name and the Model
  void create_sun(std::string const& sun_name, model const& sun_model);

  //Creating the Planet, giving it a Name and the Model 
  void create_planet(std::string const& planet_name, model const& planet_model);

  //Creating the moon and assigning it to the desired Planet 
  void create_moon_for_planet(std::string const& planet_name,
                              std::string const& moon_name,
                              model const& moon_model);

  //The Matrix that creates the Planet and gives it's relative distance and speed in the Solar system
  void process_planet_matrix(Node* planet,
                             glm::fvec3& distance,
                             glm::fmat4 const& solar_system_origin,
                             int speed_factor) const;
  
  //The Matrix that creates the moon and gives its relative distance and size 
  void process_moon_matrix(Node* moon,
                           Node* planet,
                           glm::fvec3 const& distance_from_planet,
                           glm::fvec3 const& moon_size) const;

//Rendering the Scene with all the Nodes and thier relative distances
  void render_scene(std::list<Node*> const& sol,
                    glm::fvec3& distance,
                    glm::fmat4 const& solar_system_origin) const;

  //Rendering a Node as a Planet in our Scene
  void render_node(Node* planet) const;



  //Creating a SceneGraph
  SceneGraph scene_graph;

  // cpu representation of model
  model_object planet_object;
  model_object star_object;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;


  model star_model;

  std::vector<float> m_stars;


};

#endif