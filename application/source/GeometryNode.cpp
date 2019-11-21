#include <GeometryNode.hpp>

GeometryNode::GeometryNode(std::string const& name,
                           std::string const& path,
                           std::int32_t& depth,
                           model const& geometry_model)
    : Node{name, path, depth}, geometry_{geometry_model} {};

GeometryNode::GeometryNode(std::string const& name, model const& geometry_model)
    : Node{name}, geometry_{geometry_model} {};

model GeometryNode::getGeometry() const& {
  return geometry_;
};

void GeometryNode::setGeometry(model const& geometry_model) {
  geometry_ = geometry_model;
}