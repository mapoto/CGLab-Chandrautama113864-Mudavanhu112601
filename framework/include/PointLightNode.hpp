#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include "Node.hpp"

class PointLightNode : public Node {
 private:
  float lightIntensity;
  glm::fvec3 lightColour;

 public:
  PointLightNode();
  PointLightNode(std::string const& name);
  ~PointLightNode();

  float getlightIntesity() const;
  glm::fvec3 getlightColour() const;

  void setLightColour(glm::fvec3 const& inputLightColour);
  void setLightIntensity (float inputLightIntensity);

};

#endif  // POINT_LIGHT_NODE_HPP
