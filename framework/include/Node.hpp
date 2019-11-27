#ifndef NODE_HPP
#define NODE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <list>
#include <memory>

/*
The Node Class is responsible for creating the different Nodes in the Sceen Graph.
Some A Root Node is the Ancestor node for the rest of all the nodes that that are in the tree
Each created Node can either be a Parent or Child Node. Each created node is given a name to be 
used to identify it when parsing through the tree. 
*/

/////////////////////////////////////////////////////////////////////////////////////////////

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
  //User Defined Constructor
  Node(std::string const& name);

  //Default Constructor
  Node();

  //Destructor 
  ~Node();

  //Used to Set a Node as a Parent Node
  Node* getParent() const;
  void setParent(Node* parent);

  //Used to created a List of all the Children Nodes in a Tree (Scene Graph)
  Node* getChild(std::string const& name) const;
  std::list<Node*> getChildrenList() const;

  // Used to identify the Name of the Child and get the path to the Node and see how dep it is in the Tree
  std::string getName() const;
  std::string getPath() const;
  int getDepth() const;

  //Used to set the initial Local Transformation Matrix of the Created Node/ Child/ Parent
  glm::mat4 getLocalTransform() const;
  void setLocalTransform(glm::mat4 const& inputMatrix);

  //Used to set the initial World Transformation Matrix of the Created Node/ Child/ Parent
  glm::mat4 getWorldTransform() const;
  void setWorldTransform(glm::mat4 const& inputMatrix);

  //Used to add or Creat a new Node/ Child/ Parent
  void addChild(Node* node);

  //Used to remove a Node/ Child/ Parent by specifiying the name of the Node 
  Node* removeChild(std::string const& name);
};

#endif  // NODE.HPP