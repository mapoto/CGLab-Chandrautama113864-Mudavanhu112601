#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <memory>
#include <string>

//include all useful nodes
#include "CameraNode.hpp"
#include "GeometryNode.hpp"
#include "PointLightNode.hpp"

class SceneGraph {
 public:
  SceneGraph();
  ~SceneGraph();

  void setName(std::string const& name);
  void setRoot(Node* root);

  std::string getName() const;
  Node* getRoot() const;
  std::string printGraph() const;
  
 private:
  Node* root_;
  std::string name_;
  //static SceneGraph* instance;
};

#endif  // SCENEGRAPH_HPP
