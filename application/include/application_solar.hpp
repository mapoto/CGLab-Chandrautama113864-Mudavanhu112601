#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "SceneGraph.hpp"
#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

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
  void initialize_scene_graph();
  void initializeShaderPrograms();
  void initializeGeometry(model& planet_model);

  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  void set_m_view_transform(glm::fmat4 const& cam_matrix);

 private:
  void create_camera(std::string const& camera_name);
  void create_sun(std::string const& sun_name, model const& sun_model);
  void create_planet(std::string const& planet_name, model const& planet_model);

  void create_moon_for_planet(std::string const& planet_name,
                              std::string const& moon_name);

  void render_scene() const;

  void render_planet(Node* planet) const;

  SceneGraph scene_graph;

  // cpu representation of model
  model_object planet_object;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif