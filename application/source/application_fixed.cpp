#include "application_fixed.hpp"
#include "window_handler.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SceneGraph.hpp>
#include <GeometryNode.hpp>
#include <CameraNode.hpp>

ApplicationFixed::ApplicationFixed(std::string const &resource_path)
    : Application{resource_path}
{
  glm::fmat4 projection_matrix = utils::calculate_projection_matrix(initial_aspect_ratio);
  // upload projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(projection_matrix));
}

void ApplicationFixed::render() const
{
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(model_matrix));
  // draw triangle
  glBegin(GL_TRIANGLES);
  // first vertex
  glVertex3f(0.5, -0.5, 0.0);
  glColor3f(1.0, 0.0, 0.0);
  // second vertex
  glVertex3f(-0.5, -0.5, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  // third vertes
  glVertex3f(0.0, 0.5, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glEnd();
}

// exe entry point
int main(int argc, char *argv[])
{
  Application::run<ApplicationFixed>(argc, argv, 1, 0);

  std::shared_ptr<SceneGraph> scene0 = scene0->getInstance();

  std::shared_ptr<Node> root_scene0 = scene0->getRoot();

  CameraNode cam = CameraNode{"camera_node"};
  root_scene0->addChild(cam);

  Node holder_mercury = Node{"holder_mercury"};
  Node holder_venus = Node{"holder_venus"};
  Node holder_earth = Node{"holder_earth"};
  Node holder_mars = Node{"holder_mars"};
  Node holder_jupiter = Node{"holder_jupiter"};
  Node holder_saturn = Node{"holder_saturn"};
  Node holder_uranus = Node{"holder_uranus"};
  Node holder_neptune = Node{"holder_neptune"};

  root_scene0->addChild(holder_mercury);
  root_scene0->addChild(holder_venus);
  root_scene0->addChild(holder_earth);
  root_scene0->addChild(holder_mars);
  root_scene0->addChild(holder_jupiter);
  root_scene0->addChild(holder_saturn);
  root_scene0->addChild(holder_uranus);
  root_scene0->addChild(holder_neptune);

  // GeometryNode geo_mercury = GeometryNode{"geo_mercury", };
  // GeometryNode geo_venus = GeometryNode{"geo_venus", };
  // GeometryNode geo_earth = GeometryNode{"geo_earth", };
  // GeometryNode geo_mars = GeometryNode{"geo_mars", };
  // GeometryNode geo_jupiter = GeometryNode{"geo_jupiter", };
  // GeometryNode geo_saturn = GeometryNode{"geo_saturn", };
  // GeometryNode geo_uranus = GeometryNode{"geo_uranus", };
  // GeometryNode geo_neptune = GeometryNode{"geo_neptune", };

  std::cout << root_scene0->getPath() << std::endl;
}