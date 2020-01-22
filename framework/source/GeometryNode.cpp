#include <GeometryNode.hpp>

////////////////////////////////////////////////////////////////////////////////
// The Geometry Node is ude to initialize the Planets with a Name and a Model -
// The model generates the Mesh

// User Defined Constructor
GeometryNode::GeometryNode(std::string const& name,
                           model const& geometry_model,
                           glm::fvec3 const& color,
                           pixel_data const& texture)
    : Node{name},
      geometry_{geometry_model},
      color_{color},
      texture_{texture},
      planet_texture_obj_{} {}

// Destructor
GeometryNode::~GeometryNode() {}

// Function Call that gets the Model for the Geometry
model GeometryNode::getGeometry() const {
  return geometry_;
}

// Function Call that sets the Model to the Geometry
void GeometryNode::setGeometry(model const& geometry_model) {
  geometry_ = geometry_model;
}

glm::fvec3 GeometryNode::getColor() const {
  return color_;
}

void GeometryNode::setColor(glm::fvec3 const& inputColor) {
  color_ = inputColor;
}

pixel_data GeometryNode::getTexture() const {
  return texture_;
}

void GeometryNode::setTexture(pixel_data const& input_texture) {
  texture_ = input_texture;
}

texture_object GeometryNode::getTextureObj() const {
  return planet_texture_obj_;
}
void GeometryNode::setTextureObj(texture_object const input_texture_obj) {
  planet_texture_obj_ = input_texture_obj;
}

void GeometryNode::setTextureObjAttribute(gl::GLuint const& handle, gl::GLenum const& target){
  planet_texture_obj_.handle = handle;
  planet_texture_obj_.target = target;
}
