#include "Node.hpp"

////////////////////////////////////////////////////////////////////////////////

//User defined Constructor Used to create a new Node and assign a name, a path and the depth in the Tree(Scenegraph)
Node::Node(std::string const& name)
    : name_{name}, path_{"\\" + name_}, depth_{0} {
  localTransform_ = glm::fmat4{};
  worldTransform_ = glm::fmat4{};
  parent_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

//Default Constructor Used to create a new Node and assign a defualt name, a path and a defualt depth in the Tree(Scenegraph)
Node::Node() : name_{"name"}, path_{"\\" + name_}, depth_{0} {
  localTransform_ = glm::fmat4{};
  worldTransform_ = glm::fmat4{};
  parent_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

Node::~Node() {
  // clear connection between this node and its parent if it has one
  if (parent_ != nullptr) {
    // remove this node from its parent list
    parent_->removeChild(name_);
    // set the parent as nullptr
    parent_ = nullptr;
  }

  // clear connection between this node and its children if exist
  if (!children_.empty()) {
    // set the parent of every children as nullptr
    for (auto child : children_) {
      child->parent_ = nullptr;
    }
    // clear this node list of children
    children_.clear();
  }
}

////////////////////////////////////////////////////////////////////////////////
//Used to access the Parent Node of the Child Node in a Tree 

Node* Node::getParent() const {
  return parent_;
}

////////////////////////////////////////////////////////////////////////////////
//Used to set a  Parent Node for a given Child Node in a Tree 

void Node::setParent(Node* parent) {
  parent_->addChild(this);
}

////////////////////////////////////////////////////////////////////////////////
//Used to to parse Node in a Tree and get a specific Node from the List 

Node* Node::getChild(std::string const& name) const {
  if (!children_.empty()) {
    auto it =
        std::find_if(children_.cbegin(), children_.cend(),
                     [&](Node* const node) { return node->getName() == name; });
    if (it != children_.end()) {
      return *it;
    }
  }

  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
//Used to create a List of all the Children Nodes in a Tree

std::list<Node*> Node::getChildrenList() const {
  return children_;
}

////////////////////////////////////////////////////////////////////////////////
//Function for getting the name of the Node

std::string Node::getName() const {
  return name_;
}

//Function for getting the Path to the Node

std::string Node::getPath() const {
  return path_;
}

////////////////////////////////////////////////////////////////////////////////
//Function used to get the Depth of a Node in the Tree 

int Node::getDepth() const {
  return depth_;
}

////////////////////////////////////////////////////////////////////////////////
//Function for the getting the Local Transformation matrix of the created Node Planet)

glm::mat4 Node::getLocalTransform() const {
  return localTransform_;
}

glm::fvec3 Node::getColor() const{
  return color_;
}

void Node::setColor(glm::fvec3 const& inputColor){
  color_ = inputColor;
}

////////////////////////////////////////////////////////////////////////////////
//Function for the setting the Local Transformation matrix of a new Node (Planet)

void Node::setLocalTransform(glm::mat4 const& inputMatrix) {
  localTransform_ = inputMatrix;
}

////////////////////////////////////////////////////////////////////////////////
// For getting the World Transformation of a Node or Planet

glm::mat4 Node::getWorldTransform() const {
  return worldTransform_;
}

////////////////////////////////////////////////////////////////////////////////
//
void Node::setWorldTransform(glm::mat4 const& inputMatrix) {
  worldTransform_ = inputMatrix;
}

////////////////////////////////////////////////////////////////////////////////

void Node::addChild(Node* node) {
  // push the child node to the list
  children_.push_back(node);

  // set its depth and path based on this node as its parent
  node->path_ = this->path_ + node->path_;
  node->depth_ = this->depth_ + 1;
  node->parent_ = this;
  node->worldTransform_ = this->localTransform_; // multiply with parent world trans mat
}

////////////////////////////////////////////////////////////////////////////////
//Function for deleting a Node from the Tree 

Node* Node::removeChild(std::string const& name) {
  Node* unwanted = getChild(name);

  unwanted->path_ = "\\" + name;
  unwanted->setParent(nullptr);

  children_.remove(unwanted);

  return unwanted;
}


