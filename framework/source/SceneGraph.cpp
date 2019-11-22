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
  auto const list = getRoot()->getChildrenList();

  for (auto it = list.begin(); it != list.end(); it++) {
    from_root += ("\n |_" + (*it)->getName());

    auto const list2 = (*it)->getChildrenList();
    if (!list2.empty()) {
      for (auto it2 = list2.begin(); it2 != list2.end(); it2++) {
        from_root += ("\n  |_" + (*it2)->getName());
      }
    }
  }

  return from_root;
}
