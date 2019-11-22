#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"


#include "CameraNode.hpp"
#include "GeometryNode.hpp"
#include "SceneGraph.hpp"

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

 protected:
  void create_scene_graph(model& planet_model);
  void create_planet(SceneGraph& SceneGraph, std::string planet_name, model const& planet_model);

  void create_moon_for_planet(SceneGraph& SceneGraph,
                              std::string const& planet_name,
                              std::string const& moon_name);
  void initializeShaderPrograms();
  void initializeGeometry();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  void calculate_m_view_transform();

  // cpu representation of model
  model_object planet_object;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif