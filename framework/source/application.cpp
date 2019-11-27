#include "application.hpp"

#include "utils.hpp"
#include "window_handler.hpp"
#include "shader_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//Used for updating the Shaders of the program
static void update_shader_programs(std::map<std::string, shader_program>& shaders, bool throwing);

//Used to set the Intial Resolution of the Application Window 
const glm::uvec2 Application::initial_resolution = {640u, 480u};
const float Application::initial_aspect_ratio = float(initial_resolution.x) / float(initial_resolution.y);

//Used to call the Shaders and the Path of the resources needed to Run the Application
Application::Application(std::string const& resource_path)
 :m_resource_path{resource_path}
 ,m_shaders{}
{}

//The Destructor that is used to free the Resourses used when the Application is running e.g Shaders
Application::~Application() {
  // free all shader program objects
  for (auto const& pair : m_shaders) {
    glDeleteProgram(pair.second.handle);
  }
}

void Application::reloadShaders(bool throwing) {
  // recompile shaders from source files
  update_shader_programs(m_shaders, throwing);
  // after shader programs are recompiled, uniform locations may change
  updateUniformLocations();
  // upload values to new locations
  uploadUniforms();
}

// update shader uniform locations
void Application::updateUniformLocations() {
  for (auto& pair : m_shaders) {
    for (auto& uniform : pair.second.u_locs) {
      // store uniform location in map
      uniform.second = utils::glGetUniformLocation(pair.second.handle, uniform.first.c_str());
    }
  }
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void Application::key_callback(GLFWwindow* m_window, int key, int action, int mods) {
  // handle special keys
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_window, 1);
  }
  else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    reloadShaders(false);
  }
  // else pass input to derived class
  else {
    keyCallback(key, action, mods);
  }
}

//handle mouse movement input
void Application::mouse_callback(GLFWwindow* window, double pos_x, double pos_y) {
  // pass input to derived class
  mouseCallback(pos_x, pos_y);
  // reset cursor pos to receive position delta next frame
  glfwSetCursorPos(window, 0.0, 0.0);
}

// handle window resizing
void Application::resize_callback(unsigned width, unsigned height) {
  // resize framebuffer
  glViewport(0, 0, width, height);
  // resize fbo attachments
  resizeCallback(width, height);
}
///////////////////////////// local helper functions //////////////////////////
// update uniform locations
static void update_shader_programs(std::map<std::string, shader_program>& shaders, bool throwing) {
  // actual functionality in lambda to allow update with and without throwing
  auto update_lambda = [](shader_program& program){
    // throws exception when compiling was unsuccessfull
    GLuint new_program = shader_loader::program(program.shader_paths);
    // free old shader program
    glDeleteProgram(program.handle);
    // save new shader program
    program.handle = new_program;
  };

  // reload all shader programs
  for (auto& pair : shaders) {
    if (throwing) {
      update_lambda(pair.second);
    }
    else {
      try {
       update_lambda(pair.second);
      }
      catch(std::exception&) {
        // dont crash, allow another try
      }
    }
  }
}

