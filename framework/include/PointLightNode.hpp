#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include "Node.hpp"

class PointLightNode : public Node {
 private:
  double lightIntensity;
  glm::fvec3 lightColour;

 public:
  PointLightNode();
  PointLightNode(std::string const& name);
  ~PointLightNode();

  double getLightIntesity() const;
  glm::fvec3 getlightColour() const;

  void setLightColour(glm::fvec3 const& inputLightColour);
  void setLightIntensity (double inputLightIntensity);

};

#endif  // POINT_LIGHT_NODE_HPP
