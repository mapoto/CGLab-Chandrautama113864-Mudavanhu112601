#include "Node.hpp"

Node::Node(std::string const& name)
    : name_{name}, path_{"\\" + name_}, depth_{0} {
  localTransform_ = glm::fmat4{};
  worldTransform_ = glm::fmat4{};
}

Node::Node() : name_{"name"}, path_{"\\" + name_}, depth_{0} {
  localTransform_ = glm::fmat4{};
  worldTransform_ = glm::fmat4{};
}

Node::~Node() {
  //clear connection between this node and its parent if it has one
  if (parent_ != nullptr) {
    //remove this node from its parent list
    parent_->removeChild(name_);
    //set the parent as nullptr
    parent_ = nullptr;
  }

  //clear connection between this node and its children if exist
  if (!children_.empty()) {
    //set the parent of every children as nullptr 
    for (auto child : children_) {
      child->parent_ = nullptr;
    }
    //clear this node list of children
    children_.clear();
  }
}

////////////////////////////////////////////////////////////////////////////////

Node* Node::getParent() const {
  return parent_;
}

////////////////////////////////////////////////////////////////////////////////

void Node::setParent(Node* parent) {
  parent_->addChild(this);
}

////////////////////////////////////////////////////////////////////////////////

Node* Node::getChild(std::string const& name) const {
  auto it =
      std::find_if(children_.begin(), children_.end(), [name](Node* node) {
        if (node->getName() == name) {
          return node;
        };
      });

  return *(it);
}

////////////////////////////////////////////////////////////////////////////////

std::list<Node*> Node::getChildrenList() const {
  return children_;
}

////////////////////////////////////////////////////////////////////////////////

std::string Node::getName() const {
  return name_;
}

std::string Node::getPath() const {
  return path_;
}

////////////////////////////////////////////////////////////////////////////////

int Node::getDepth() const {
  return depth_;
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 Node::getLocalTransform() const {
  return localTransform_;
}

////////////////////////////////////////////////////////////////////////////////

void Node::setLocalTransform(glm::mat4 const& inputMatrix) {
  localTransform_ = inputMatrix;
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 Node::getWorldTransform() const {
  return worldTransform_;
}

////////////////////////////////////////////////////////////////////////////////

void Node::setWorldTransform(glm::mat4 const& inputMatrix) {
  worldTransform_ = inputMatrix;
}

////////////////////////////////////////////////////////////////////////////////

void Node::addChild(Node* node) {
  //push the child node to the list
  children_.push_back(node);

  //set its depth and path based on this node as its parent
  node->path_ = this->path_ + node->path_;
  node->depth_ = this->depth_ + 1;
  node->parent_ = this;
  node->worldTransform_ = this->localTransform_;
}

////////////////////////////////////////////////////////////////////////////////

Node* Node::removeChild(std::string const& name) {
  Node* unwanted = getChild(name);

  unwanted->path_ = "\\" + name;
  unwanted->setParent(nullptr);

  children_.remove(unwanted);

  return unwanted;
}