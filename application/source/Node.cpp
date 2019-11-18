#include <Node.hpp>

Node::Node(std::string const &name,
           std::string const &path,
           int &depth) : name_{name},
                         path_{path},
                         depth_{depth} {};

////////////////////////////////////////////////////////////////////////////////

Node::Node(std::string const &name) : name_{name}, depth_{getParent()->depth_++}, path_{getParent()->getPath() + "'\'" + name_} {};

Node *Node::getParent() const
{
    return parent_;
}

////////////////////////////////////////////////////////////////////////////////

void Node::setParent(Node *parent)
{
    parent_ = parent;
    parent->addChild(*this);
};

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<Node> Node::getChild(std::string const &name) const
{
    std::shared_ptr<Node> child = nullptr;
    std::for_each(children_.begin, children_.end, [name](std::shared_ptr<Node> node) {
        if (node->getName() == name)
        {
            return node;
        }
    });

    return child;
}

////////////////////////////////////////////////////////////////////////////////

std::list<std::shared_ptr<Node>> Node::getChildrenList() const
{
    return children_;
}

////////////////////////////////////////////////////////////////////////////////

std::string Node::getName() const
{
    return name_;
}

std::string Node::getPath() const
{
    return path_;
}

////////////////////////////////////////////////////////////////////////////////

int Node::getDepth() const
{
    return depth_;
};

////////////////////////////////////////////////////////////////////////////////

glm::mat4 Node::getLocalTransform() const
{
    return localTransform_;
};

////////////////////////////////////////////////////////////////////////////////

void Node::setLocalTransform(glm::mat4 const &inputMatrix)
{
    localTransform_ = inputMatrix;
}

////////////////////////////////////////////////////////////////////////////////

glm::mat4 Node::getWorldTransform() const
{
    return worldTransform_;
};

////////////////////////////////////////////////////////////////////////////////

void Node::setLocalTransform(glm::mat4 const &inputMatrix)
{
    worldTransform_ = inputMatrix;
};

////////////////////////////////////////////////////////////////////////////////

void Node::addChild(Node &node)
{
    children_.push_back(std::make_shared<Node>(node));
    node.setParent(this);
};

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<Node> Node::removeChild(std::string const &name)
{

    std::shared_ptr<Node> removed = getChild(name);

    children_.remove_if(
        [name](Node *node) {
            return node->getName() == name;
        });

    return removed;
}
