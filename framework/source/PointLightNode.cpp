#include "PointLightNode.hpp"

PointLightNode::PointLightNode() : Node{} {}
PointLightNode::PointLightNode(std::string const& name) : Node{name} {}
PointLightNode::~PointLightNode() {}

double PointLightNode::getLightIntesity() const {
  return lightIntensity;
}

glm::fvec3 PointLightNode::getlightColour() const {
  return lightColour;
}

void PointLightNode::setLightColour(glm::fvec3 const& inputLightColour) {
  lightColour = inputLightColour;
}

void PointLightNode::setLightIntensity(double inputLightIntensity) {
  lightIntensity = inputLightIntensity;
}
