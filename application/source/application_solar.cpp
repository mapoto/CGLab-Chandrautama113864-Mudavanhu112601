#include "application_solar.hpp"
#include "window_handler.hpp"

#include "model_loader.hpp"
#include "shader_loader.hpp"
#include "texture_loader.hpp"

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

#include <ctime>
#include <iostream>

/* ----------------------- constructor and destructor ----------------------- */

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
    : Application{resource_path},
      scene_graph{},
      planet_object{},
      star_object{},
      orbit_object{},
      skybox_object{},
      screenquad_object{},
      skybox_texture_object{0, GL_TEXTURE_CUBE_MAP},
      skybox_textures{},
      FB_color_attachment{},
      FB_depth_attachment{},
      framebuffer{},
      m_view_transform{},
      m_view_projection{
          utils::calculate_projection_matrix(initial_aspect_ratio)} {
  initialize_scene_graph();
  initialize_stars(3000);
  initialize_orbits(720);
  initializeTextures();
  initializeSkybox();
  initializeScreenQuad();
  initializeFramebuffer();
  initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);
}

/* ----------------- Rendering the Solar System Application ----------------- */

void ApplicationSolar::render() const {
  // ---- Bind Framebuffer Object to render the scene to it ----
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);
  // clear Framebuffer Attachments before drawing them
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // list of nodes in graph below the root including sun, camera and planets
  auto const solar_system = scene_graph.getRoot();
  // a pivot for planets' revolution = sun's world_matrix
  glm::fmat4 const solar_system_origin =
      scene_graph.getRoot()->getWorldTransform();

  render_scene(solar_system, solar_system_origin);
  render_stars();
  // render_orbits();
  // render_skybox();
  renderScreenQuad();
}
// render Stars
void ApplicationSolar::render_stars() const {
  glUseProgram(m_shaders.at("stars").handle);
  glBindVertexArray(star_object.vertex_AO);
  glPointSize(3.0);
  glDrawArrays(star_object.draw_mode, gl::GLint(0), star_object.num_elements);
}

// render Stars
void ApplicationSolar::render_orbits() const {
  glUseProgram(m_shaders.at("orbits").handle);
  glBindVertexArray(orbit_object.vertex_AO);
  glPointSize(10.0);
  glDrawArrays(orbit_object.draw_mode, gl::GLint(0), orbit_object.num_elements);
}

// Rendering all the Nodes in the Scene by looping through the Tree (SceneGraph)
void ApplicationSolar::render_scene(
    Node* root,
    glm::fmat4 const& solar_system_origin) const {
  int planet_rotation_speed_factor = 1;
  glm::vec4 origin = solar_system_origin * glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec3 distance = glm::vec3{origin.x, origin.y, origin.z};

  auto sol = root->getChildrenList();
  PointLightNode* sun_temp = static_cast<PointLightNode*>(
      root->getChild("holder_sun")->getChild("point_light"));
  sun_temp->setWorldTransform(solar_system_origin);

  uint32_t texture_index = 0;
  // loop through all elements below the root
  for (auto planet : sol) {
    // ignore rendering the camera
    if (planet->getName() != "camera") {
      // calculate the matrix of each planet
      process_planet_matrix(planet, distance, solar_system_origin,
                            planet_rotation_speed_factor);

      // use it to get the image for this frame
      render_planet(planet, sun_temp, texture_index);

      auto moons = planet->getChildrenList();
      uint32_t moon_texture_index = sol.size();
      if (!moons.empty()) {
        glm::fvec3 moon_distance_from_planet = glm::fvec3{2.0f, 0.0f, 0.0f};
        glm::fvec3 moon_size = glm::fvec3{0.5f};

        for (auto moon : moons) {
          if (moon->getName() == "holder_moon") {
            process_moon_matrix(moon, planet, moon_distance_from_planet,
                                moon_size);

            render_planet(moon, sun_temp, moon_texture_index);
            ++moon_texture_index;
          }
        }
      }

      // lazy increment for the next planet
      distance += glm::fvec3{4.0f, 0.0f, 0.0f};
      ++planet_rotation_speed_factor;
      ++texture_index;
    }
  }
}

// Rendering the Planet using the shader
void ApplicationSolar::render_planet(Node* planet,
                                     PointLightNode* point_light,
                                     uint32_t const planet_index) const {
  GeometryNode* planet_geo =
      static_cast<GeometryNode*>(planet->getChildrenList().front());

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

  // draw planet with color
  glUniform3f(
      glGetUniformLocation(m_shaders.at("planet").handle, "planet_Color"),
      planet_geo->getColor().x, planet_geo->getColor().y,
      planet_geo->getColor().z);

  // setup the lighting properties
  glUniform3f(
      glGetUniformLocation(m_shaders.at("planet").handle, "light_Color"),
      point_light->getlightColour().x, point_light->getlightColour().y,
      point_light->getlightColour().z);

  glUniform1f(
      glGetUniformLocation(m_shaders.at("planet").handle, "light_Intensity"),
      point_light->getlightIntesity());

  glm::fvec4 light_position =
      (point_light->getWorldTransform() * glm::fvec4(0.0f, 0.0f, 0.0f, 1.0f));

  glUniform3f(glGetUniformLocation(m_shaders.at("planet").handle, "light_Pos"),
              light_position.x, light_position.y, light_position.z);

  // draw the planet's texture
  texture_object planet_texture_object = planet_geo->getTextureObj();

  glActiveTexture(GL_TEXTURE1 + planet_index);
  glBindTexture(planet_texture_object.target, planet_texture_object.handle);

  glUniform1i(
      glGetUniformLocation(m_shaders.at("planet").handle, "planet_Texture"),
      planet_texture_object.handle);

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements,
                 model::INDEX.type, NULL);
}

void ApplicationSolar::render_skybox() const {
  // disable writing to the depth buffers (to draw transparent objects like
  // skybox)
  glDepthMask(GL_FALSE);

  glUseProgram(m_shaders.at("skybox").handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_object.handle);
  glUniform1i(glGetUniformLocation(m_shaders.at("skybox").handle, "SkyTexture"),
              0);

  // scale skybox
  glm::fmat4 model_matrix = glm::fmat4{1.0};
  model_matrix = glm::scale(model_matrix, glm::fvec3{40});
  // give matrices to shaders
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ModelMatrix"), 1,
                     GL_FALSE, glm::value_ptr(model_matrix));

  glBindVertexArray(skybox_object.vertex_AO);
  glDrawElements(skybox_object.draw_mode, skybox_object.num_elements,
                 model::INDEX.type, NULL);

  // enable writing to depth buffer again so the non-transparent objects
  // (planets) can be rendered
  glDepthMask(GL_TRUE);
}

void ApplicationSolar::renderScreenQuad() const {
  // bind to default framebuffer at 0
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glUseProgram(m_shaders.at("screenquad").handle);
  glActiveTexture(GL_TEXTURE2);  // texture from framebuffer is in slot 2
  glBindTexture(GL_TEXTURE_2D, FB_color_attachment.handle);
  // upload texture from framebuffer object to shader
  glUniform1i(m_shaders.at("screenquad").u_locs.at("FBTexture"), 2);

  glBindVertexArray(screenquad_object.vertex_AO);
  glDrawArrays(screenquad_object.draw_mode, 0, screenquad_object.num_elements);
}

/* ----------------------- calculate transform matrix ----------------------- */

// calculate planet world matrix
void ApplicationSolar::process_planet_matrix(
    Node* planet,
    glm::fvec3& distance,
    glm::fmat4 const& solar_system_origin,
    int speed_factor) const {
  // calculate the planet's matrix when it's translated to the correct
  // distance from the root.
  glm::fmat4 planet_matrix;

  if (planet->getName() == "holder_sun") {
    planet_matrix = glm::rotate(solar_system_origin, float(glfwGetTime()),
                                glm::fvec3{0.0f, 1.0f, 0.0f}) *
                    glm::scale(glm::fvec3{1.8f});
  } else {
    /* -------------------- planet revolution around the sun
     * -------------------- */
    planet_matrix = glm::rotate(solar_system_origin,
                                2 * float(glfwGetTime() / speed_factor),
                                glm::fvec3{0.0f, 1.0f, 0.0f}) *
                    glm::scale(glm::fvec3{1.8f}) * glm::translate(distance);

    /* --------------------- planet rotation around its axis
     * -------------------- */
    planet_matrix =
        glm::rotate(planet_matrix, 10 * float(glfwGetTime() / speed_factor),
                    glm::fvec3{0.0f, 1.0f, 0.0f});
  }
  planet->setWorldTransform(planet_matrix);
}

// The Moon Matrix with relativity to its Parent Planet
void ApplicationSolar::process_moon_matrix(
    Node* moon,
    Node* planet,
    glm::fvec3 const& distance_from_planet,
    glm::fvec3 const& moon_size) const {
  // multiplication to bring it to the same world coordinate system
  glm::fmat4 moon_matrix =
      glm::rotate(planet->getWorldTransform(), float(glfwGetTime()),
                  glm::fvec3{0.0f, 1.0f, 0.0f}) *
      glm::translate(distance_from_planet) * glm::scale(moon_size);
  moon_matrix = glm::rotate(moon_matrix, 10 * float(glfwGetTime() / 2),
                            glm::fvec3{0.0f, 1.0f, 0.0f});

  moon->setWorldTransform(moon_matrix);
}

/* --------------------- setting the view of the Camera --------------------- */

void ApplicationSolar::set_m_view_transform(glm::fmat4 const& cam_matrix) {
  m_view_transform = cam_matrix;
}

/* ----------------------------- upload shaders ----------------------------- */

// Updating the new view of the Camera
void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be
  // inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);

  glUseProgram(m_shaders.at("planet").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"), 1,
                     GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("stars").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ModelViewMatrix"), 1,
                     GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("orbit").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"), 1, GL_FALSE,
                     glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"), 1,
                     GL_FALSE, glm::value_ptr(view_matrix));
}

// Uploading the Projection to be processed by the GPU from the Memory
void ApplicationSolar::uploadProjection() {
  glUseProgram(m_shaders.at("planet").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"), 1,
                     GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("stars").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ProjectionMatrix"), 1,
                     GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("orbit").handle);
  // upload star projection matrix to gpu
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"), 1,
                     GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"), 1,
                     GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  // upload uniform values to std::make_shared< loca>(ions
  uploadView();
  uploadProjection();
}

/* ------------------------ initialization functions ------------------------ */

void ApplicationSolar::initializeScreenQuad() {
  model screenquad_model =
      model_loader::obj(m_resource_path + "models/quad.obj", model::TEXCOORD);

  // generate vertex array object
  glGenVertexArrays(1, &screenquad_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(screenquad_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &screenquad_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
  // configure currently bound array buffer
  // Buffer mit Vertexdaten werden "Verknüpft" so dass der Renderer weiß, wo sie
  // liegen
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenquad_model.data.size(),
               screenquad_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type,
                        GL_FALSE, screenquad_model.vertex_bytes,
                        screenquad_model.offsets[model::POSITION]);

  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type,
                        GL_FALSE, screenquad_model.vertex_bytes,
                        screenquad_model.offsets[model::TEXCOORD]);

  // store type of primitive to draw
  screenquad_object.draw_mode = GL_TRIANGLE_STRIP;
  // transfer number of indices to model object
  screenquad_object.num_elements = GLsizei(screenquad_model.indices.size());
}
void ApplicationSolar::initializeFramebuffer(unsigned width, unsigned height) {
  glActiveTexture(GL_TEXTURE2);  // 0 is for textures, 1 for normalmapping

  /* ------------------------ init the color attachment -----------------------
   */
  glGenTextures(1, &FB_color_attachment.handle);
  glBindTexture(GL_TEXTURE_2D, FB_color_attachment.handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);  // scale down
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
      GL_LINEAR);  // scale up (render texture on area bigger than the texture)

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);  // MIRRORED_REPEAT
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);  // MIRRORED_REPEAT

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

  /* ------------------------ init the Depth Attachment -----------------------
   */
  glGenRenderbuffers(1, &FB_depth_attachment.handle);
  glBindRenderbuffer(GL_RENDERBUFFER, FB_depth_attachment.handle);

  // Parameters are: GL_RENDERBUFFER, internalformat (here depth format?),
  // width, height
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

  // -------- then use both to create the Framebuffer --------
  // Define Framebuffer
  glGenFramebuffers(1, &framebuffer.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);
  // Define Attachments
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                       FB_color_attachment.handle, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, FB_depth_attachment.handle);
  // Define which Buffers to write
  GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, draw_buffers);
  // check that the framebuffer can be written
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR, Framebuffer can't be written "
              << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
  }
}

void ApplicationSolar::initializeTextures() {
  auto const& planets = scene_graph.getRoot()->getChildrenList();

  // initialize the texture object for the planet
  uint32_t texture_index = 0;
  for (auto planet : planets) {
    if (!(planet->getName() == "camera")) {
      GeometryNode* planet_geo =
          static_cast<GeometryNode*>(planet->getChildrenList().front());

      auto planet_texture = planet_geo->getTexture();
      planet_geo->setTextureObjAttribute(0, GL_TEXTURE_2D);

      auto texture_object = planet_geo->getTextureObj();

      glActiveTexture(GL_TEXTURE1 + texture_index);
      glGenTextures(1, &(texture_object.handle));

      glBindTexture(texture_object.target, texture_object.handle);

      glTexParameteri(texture_object.target, GL_TEXTURE_WRAP_S,
                      GL_CLAMP_TO_EDGE);
      glTexParameteri(texture_object.target, GL_TEXTURE_WRAP_T,
                      GL_CLAMP_TO_EDGE);

      glTexParameteri(texture_object.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(texture_object.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(texture_object.target, 0, planet_texture.channels,
                   planet_texture.width, planet_texture.height, 0,
                   planet_texture.channels, planet_texture.channel_type,
                   planet_texture.ptr());
      glGenerateMipmap(texture_object.target);

      planet_geo->setTextureObj(texture_object);

      // initialize the texture object for the moon if exist
      uint32_t moon_texture_index = planets.size();

      auto moon = planet->getChild("holder_moon");
      if (moon != nullptr) {
        auto moon_geo =
            static_cast<GeometryNode*>(moon->getChildrenList().front());

        auto moon_texture = moon_geo->getTexture();
        moon_geo->setTextureObjAttribute(0, GL_TEXTURE_2D);

        auto moon_texture_obj = moon_geo->getTextureObj();

        glActiveTexture(GL_TEXTURE1 + moon_texture_index);
        glGenTextures(1, &moon_texture_obj.handle);
        glBindTexture(GL_TEXTURE_2D, moon_texture_obj.handle);

        glTexParameteri(texture_object.target, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(texture_object.target, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR);  // scale down
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);  // scale up (render texture on area bigger
                                     // than the texture)

        glTexImage2D(GL_TEXTURE_2D, 0, moon_texture.channels,
                     moon_texture.width, moon_texture.height, 0,
                     moon_texture.channels, moon_texture.channel_type,
                     moon_texture.ptr());
      }

      ++texture_index;
    }
  }
}

// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace(
      "planet",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/planet.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/planet.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  // store shader program stars in container
  m_shaders.emplace(
      "stars",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/vao.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/vao.frag"}}});

  // request uniform locations for shader program
  m_shaders.at("stars").u_locs["ModelViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;

  m_shaders.emplace(
      "orbit",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/orbits.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
  m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

  m_shaders.emplace(
      "skybox",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/skybox.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});
  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ModelMatrix"] = -1;
  m_shaders.at("skybox").u_locs["SkyTexture"] = -1;

  m_shaders.emplace(
      "screenquad",
      shader_program{
          {{GL_VERTEX_SHADER, m_resource_path + "shaders/screenquad.vert"},
           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/screenquad.frag"}}});

  m_shaders.at("screenquad").u_locs["FBTexture"] = -1;
}

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
  create_sun("holder_sun", planet_model, glm::fvec3{1.0f, 1.0f, 1.0f});
  create_planet("holder_mercury", planet_model, glm::fvec3{1.0f, 1.0f, 0.3f},
                "mercurymap.png");
  create_planet("holder_venus", planet_model, glm::fvec3{0.8f, 0.1f, 0.4f},
                "venusmap.png");
  create_planet("holder_earth", planet_model, glm::fvec3{0.1f, 1.0f, 0.8f},
                "earthmap1k.png");
  create_planet("holder_mars", planet_model, glm::fvec3{0.8f, 0.2f, 0.7f},
                "mars_1k_color.png");
  create_planet("holder_jupiter", planet_model, glm::fvec3{0.8f, 1.0f, 0.1f},
                "jupitermap.png");
  create_planet("holder_saturn", planet_model, glm::fvec3{0.8f, 0.4f, 0.6f},
                "saturnmap.png");
  create_planet("holder_uranus", planet_model, glm::fvec3{0.6f, 0.7f, 0.2f},
                "uranusmap.png");
  create_planet("holdqer_neptune", planet_model, glm::fvec3{0.3f, 0.3f, 0.7f},
                "neptunemap.png");

  // Craeting the Moon's obit and attaching it to the Earths Orbit
  create_moon_for_planet("holder_earth", "holder_moon", planet_model,
                         glm::fvec3{0.3, 0.3, 0.8}, "moonmap1k.png");

  // Printing the Scenegraph
  std::cout << scene_graph.printGraph() << std::endl;
}

void ApplicationSolar::initialize_stars(unsigned int const stars_count) {
  std::vector<gl::GLfloat> star_vector;
  std::srand(std::time(nullptr));
  star_vector.resize(stars_count * 6);
  for (unsigned int i = 0; i < stars_count; ++i) {
    for (unsigned int index = 0; index < 3; ++index) {
      gl::GLfloat coord_elem =
          static_cast<float>(std::rand() % 100) - (50.0f / 2);
      gl::GLfloat color_elem =
          static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
      star_vector.insert(star_vector.begin() + (i * 6 + index), coord_elem);
      star_vector.insert(star_vector.begin() + (i * 6 + index) + 3, color_elem);
    }
  }

  initializeGeometry(star_vector, 1);
}

void ApplicationSolar::initializeSkybox() {
  /* ------------------------- initialize skybox model ------------------------
   */
  model skybox_model =
      model_loader::obj(m_resource_path + "models/skybox.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &skybox_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(skybox_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &skybox_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_model.data.size(),
               skybox_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type,
                        GL_FALSE, skybox_model.vertex_bytes,
                        skybox_model.offsets[model::POSITION]);

  // generate generic buffer
  glGenBuffers(1, &skybox_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               model::INDEX.size * skybox_model.indices.size(),
               skybox_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  skybox_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object
  skybox_object.num_elements = GLsizei(skybox_model.indices.size());

  /* ------------------------ initialize skybox texture -----------------------
   */
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &skybox_texture_object.handle);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_object.handle);

  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_back.png"));
  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_down.png"));
  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_front.png"));
  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_left.png"));
  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_right.png"));
  skybox_textures.push_back(
      texture_loader::file(m_resource_path + "textures/skybox_up.png"));

  for (uint i = 0; i < skybox_textures.size(); i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 skybox_textures[i].channels, skybox_textures[i].width,
                 skybox_textures[i].height, 0, skybox_textures[i].channels,
                 skybox_textures[i].channel_type, skybox_textures[i].ptr());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);  // scale down
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);  // scale up (render texture on area bigger than
                                 // the texture)
  }
}

void ApplicationSolar::initialize_orbits(unsigned int const num) {
  std::vector<GLfloat> orbits;
  for (unsigned int i = 0; i < num; ++i) {
    std::vector<GLfloat> xyz{cosf(i * M_PI / (num / 2)), 0,
                             sinf(i * M_PI / (num / 2))};
    orbits.insert(orbits.begin() + 3 * i, xyz.begin(), xyz.end());
  }

  initializeGeometry(orbits, 2);
}

// initializeGeometry when there is model to be used
void ApplicationSolar::initializeGeometry(model& planet_model) {
  planet_model = model_loader::obj(m_resource_path + "models/sphere.obj",
                                   model::NORMAL | model::TEXCOORD);

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

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type,
                        GL_FALSE, planet_model.vertex_bytes,
                        planet_model.offsets[model::TEXCOORD]);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

// initializeGeometry when only array of data is available
void ApplicationSolar::initializeGeometry(
    std::vector<GLfloat> const& data_vector,
    unsigned int const& index) {
  switch (index) {
    case 1:
      // generate vertex array object for the stars
      glGenVertexArrays(1, &star_object.vertex_AO);
      // binding the array for attaching buffers
      glBindVertexArray(star_object.vertex_AO);

      // generate generic Buffer
      glGenBuffers(1, &star_object.vertex_BO);
      // bind this as an vertex array buffer containing all attributes
      glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
      // configure currently bound array buffer
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data_vector.size(),
                   data_vector.data(), GL_STATIC_DRAW);

      // active first attribute on gpu
      glEnableVertexAttribArray(0);

      // first attribute is 3 floats with no offset & stride
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            GLsizei(sizeof(float) * 6), 0);

      // active first attribute on gpu
      glEnableVertexAttribArray(1);
      // first attribute is 3 floats with no offset & stride
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                            GLsizei(sizeof(float) * 6),
                            (void*)(sizeof(float) * 3));

      star_object.draw_mode = GL_POINTS;

      star_object.num_elements = GLsizei(data_vector.size() / 6);

      break;

    case 2:
      // generate vertex array object for the stars
      glGenVertexArrays(1, &orbit_object.vertex_AO);
      // binding the array for attaching buffers
      glBindVertexArray(orbit_object.vertex_AO);

      // generate generic Buffer
      glGenBuffers(1, &orbit_object.vertex_BO);
      // bind this as an vertex array buffer containing all attributes
      glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
      // configure currently bound array buffer
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data_vector.size(),
                   data_vector.data(), GL_STATIC_DRAW);

      // active first attribute on gpu
      glEnableVertexAttribArray(0);

      // first attribute is 3 floats with no offset & stride
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            GLsizei(sizeof(float) * 3), 0);

      orbit_object.draw_mode = GL_LINE_LOOP;

      orbit_object.num_elements = GLsizei(data_vector.size() / 3);

      break;

    default:
      break;
  }
}

/* ------------------------- Node creation functions ------------------------ */

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
  cam_world_matrix = glm::translate(glm::fvec3{0.0f, 50.0f, 0.0F});

  // rotate the camera so it is facing downwards
  cam_world_matrix =
      glm::rotate(cam_world_matrix, 3.14f / 2, glm::vec3{-1.0f, 0.0f, 0.0f});

  // the new transformation to the camera with respect to the world
  cam->setWorldTransform(cam_world_matrix);

  // use the result as view transform matrix for the viewport used by
  // application
  set_m_view_transform(cam_world_matrix);
}

// create sun node with sun_name as its name and take loaded model
void ApplicationSolar::create_sun(std::string const& sun_name,
                                  model const& sun_model,
                                  glm::fvec3 const& sun_color) {
  // As a normal node pointer until light is fully implemented
  Node* sun_holder = new Node{sun_name};

  GeometryNode* sun_geometry = new GeometryNode{
      "sun_geometry", sun_model, sun_color,
      texture_loader::file(m_resource_path + "textures/sunmap.png")};

  // Create its the point light
  PointLightNode* sun_point_light = new PointLightNode{"point_light"};

  sun_point_light->setLightColour(glm::vec3(1.0f, 1.0f, 0.8f));
  sun_point_light->setLightIntensity(0.5f);

  // Attach the sun node to the root directly
  scene_graph.getRoot()->addChild(sun_holder);

  // Attach its geometry to the sun node
  sun_holder->addChild(sun_geometry);
  sun_holder->addChild(sun_point_light);
}

// create a planet node with planet_name and a loaded model for its geometry
void ApplicationSolar::create_planet(std::string const& planet_name,
                                     model const& planet_model,
                                     glm::fvec3 const& planet_color,
                                     std::string const& texture_name) {
  // Create it as node pointer to prevent it being destroyed
  Node* planet = new Node{planet_name};

  // Attach the planet node to the root directly
  scene_graph.getRoot()->addChild(planet);

  // Create its the geometry
  GeometryNode* geometry = new GeometryNode{
      "geometry_" + planet_name, planet_model, planet_color,
      texture_loader::file(m_resource_path + "textures/" + texture_name)};

  // Attach its geometry to the planet node
  planet->addChild(geometry);
}

// Create a moon for a planet using its name
void ApplicationSolar::create_moon_for_planet(std::string const& planet_name,
                                              std::string const& moon_name,
                                              model const& moon_model,
                                              glm::fvec3 const& moon_color,
                                              std::string const& texture_name) {
  // find the planet by its name and assign it to a in place variable
  auto wanted_planet = (scene_graph.getRoot())->getChild(planet_name);

  if (wanted_planet != nullptr) {
    // Create it as node pointer to prevent it being destroyed
    Node* moon = new Node{moon_name};
    wanted_planet->addChild(moon);

    // Create its the geometry with the model
    GeometryNode* moon_geometry = new GeometryNode{
        "geometry_" + moon_name, moon_model, moon_color,
        texture_loader::file(m_resource_path + "textures/" + texture_name)};

    // add the geometry to the moon
    moon->addChild(moon_geometry);
  }
}

/* ------------------ callback functions for window events ------------------ */

// handle key inputs
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  // WSAD camera movement
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

  // ARROW KEYS FOR CONTROL
  else if (key == GLFW_KEY_RIGHT &&
           (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform =
        glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    uploadView();
    std::cout << "key Right Arrow pressed: camera Angle Move Right"
              << std::endl;
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

  // scene rotation
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
  // recalculate projection matrix for new aspect ratio
  m_view_projection =
      utils::calculate_projection_matrix(float(width) / float(height));
  initializeFramebuffer(width, height);
  // upload new projection matrix
  uploadProjection();
}

/* ----------------------------- exe entry point ---------------------------- */

int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
