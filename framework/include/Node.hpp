#ifndef NODE_HPP
#define NODE_HPP


#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

// dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <list>
#include <memory>

class Node {
 protected:
  Node* parent_;
  std::list<Node*> children_;
  std::string name_;
  std::string path_;
  int depth_;
  glm::mat4 localTransform_;
  glm::mat4 worldTransform_;

 public:
  Node(std::string const& name);
  Node();
  ~Node();

  Node* getParent() const;
  void setParent(Node* parent);

  Node* getChild(std::string const& name) const;
  std::list<Node*> getChildrenList() const;

  std::string getName() const;
  std::string getPath() const;
  int getDepth() const;

  glm::mat4 getLocalTransform() const;
  void setLocalTransform(glm::mat4 const& inputMatrix);

  glm::mat4 getWorldTransform() const;
  void setWorldTransform(glm::mat4 const& inputMatrix);

  void addChild(Node* node);
  Node* removeChild(std::string const& name);
};

#endif  // NODE.HPP