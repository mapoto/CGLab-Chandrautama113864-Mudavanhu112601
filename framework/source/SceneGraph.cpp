#include <SceneGraph.hpp>

SceneGraph::SceneGraph() {
  root_ = nullptr;
}

SceneGraph::~SceneGraph() {
  root_ = nullptr;
}

void SceneGraph::setName(std::string const& name) {
  name_ = name;
}

void SceneGraph::setRoot(Node* root) {
  root_ = root;
}

std::string SceneGraph::getName() const {
  return name_;
}

Node* SceneGraph::getRoot() const {
  return root_;
}

std::string SceneGraph::printGraph() const {
  std::string from_root = getRoot()->getName();

  // std::for_each(getRoot()->getChildrenList().begin(),
  //               getRoot()->getChildrenList().end(),
  //               [&](Node* n) { from_root += ("\n \t" + n->getName()); });

  // for (auto it = getRoot()->getChildrenList().begin();
  //      it != getRoot()->getChildrenList().end(); it++) {
  //   from_root += "\n \t" + (*it)->getName();

  //   for (auto a = (*it)->getChildrenList().begin();
  //        a != (*it)->getChildrenList().end(); a++) {
  //     from_root += "\n \t" + (*a)->getName();
  //   }
  // }

  return from_root;
}
