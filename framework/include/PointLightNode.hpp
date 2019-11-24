#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include "Node.hpp"

class PointLightNode : public Node {
 private:
 public:
  PointLightNode();
  PointLightNode(std::string const& name);
  ~PointLightNode();
};

#endif // POINT_LIGHT_NODE_HPP
