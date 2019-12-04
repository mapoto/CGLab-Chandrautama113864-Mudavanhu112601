#include <GeometryNode.hpp>

////////////////////////////////////////////////////////////////////////////////
//The Geometry Node is ude to initialize the Planets with a Name and a Model - The model generates the Mesh

//User Defined Constructor
GeometryNode::GeometryNode(std::string const& name, model const& geometry_model)
    : Node{name}, geometry_{geometry_model} {}

//Destructor
GeometryNode::~GeometryNode(){}

//Function Call that gets the Model for the Geometry
model GeometryNode::getGeometry() const {
  return geometry_;
}

//Function Call that sets the Model to the Geometry
void GeometryNode::setGeometry(model const& geometry_model) {
  geometry_ = geometry_model;
}