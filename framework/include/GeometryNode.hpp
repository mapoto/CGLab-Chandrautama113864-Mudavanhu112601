#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <Node.hpp>
#include <model.hpp>
#include <pixel_data.hpp>
#include "structs.hpp"

///////////////////////////////////////////////////////////////////

class GeometryNode : public Node {
 private:
  model geometry_;
  glm::fvec3 color_;
  pixel_data texture_;
  texture_object planet_texture_obj_;

 public:
  // User Defined Constructor of the GeometryNode
  GeometryNode(std::string const& name,
               model const& geometry_model,
               glm::fvec3 const& color,
               pixel_data const& texture);

  // Destructor of the GeometryNode
  ~GeometryNode();

  // Getter and Setter Functions for the GeometryNode
  model getGeometry() const;
  void setGeometry(model const& geometry_model);

  glm::fvec3 getColor() const;
  void setColor(glm::fvec3 const& inputColor);

  pixel_data getTexture() const;
  void setTexture(pixel_data const& input_texture);

  texture_object getTextureObj() const;
  void setTextureObj(texture_object const input_texture_obj);
  void setTextureObjAttribute(gl::GLuint const& handle, gl::GLenum const& target);
  
};

#endif  // GEOMETRY_NODE_HPP