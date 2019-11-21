#include <SceneGraph.hpp>

SceneGraph* SceneGraph::singletonSceneGraph = NULL;

SceneGraph::SceneGraph() {
  if (!getInstance()) {
    setName("default");
    setRoot(std::make_shared<Node>("root", "/", 0));
    singletonSceneGraph = this;
  }
};

SceneGraph::~SceneGraph(){
};

void SceneGraph::setName(std::string const& name) {
  name_ = name;
};

void SceneGraph::setRoot(std::shared_ptr<Node> const& root) {
  root_ = root;
};

SceneGraph* SceneGraph::getInstance() {
  if (!singletonSceneGraph) {
    singletonSceneGraph = new SceneGraph();
  }
  return singletonSceneGraph;
}

std::string SceneGraph::getName() const {
  return name_;
};

std::shared_ptr<Node> SceneGraph::getRoot() const {
  return root_;
};

std::string SceneGraph::printGraph() const {
  return "hello";
}
