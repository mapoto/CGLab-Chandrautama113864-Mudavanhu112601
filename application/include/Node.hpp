#ifndef NODE_HPP
#define NODE_HPP

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
  std::list<std::shared_ptr<Node>> children_;
  std::string name_;
  std::string path_;
  int depth_;
  glm::mat4 localTransform_;
  glm::mat4 worldTransform_;

 public:
  Node(std::string const& name, std::string const& path, std::int32_t& depth);
  Node(std::string const& name);

  ~Node();

  Node* getParent() const;
  void setParent(Node* parent);

  std::shared_ptr<Node> getChild(std::string const& name) const;
  std::list<std::shared_ptr<Node>> getChildrenList() const;

  std::string getName() const;
  std::string getPath() const;
  int getDepth() const;

  glm::mat4 getLocalTransform() const;
  void setLocalTransform(glm::mat4 const& inputMatrix);

  glm::mat4 getWorldTransform() const;
  void setWorldTransform(glm::mat4 const& inputMatrix);

  void addChild(Node& node);
  std::shared_ptr<Node> removeChild(std::string const& name);
};

#endif  // NODE.HPP