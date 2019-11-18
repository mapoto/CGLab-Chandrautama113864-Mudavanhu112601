#include <SceneGraph.hpp>

SceneGraph::SceneGraph(std::string const &name){
    setName(name);
    setRoot(std::make_shared<Node>("root","/", 0));
};


void SceneGraph::setName(std::string const &name){
    name_ = name;
};

void SceneGraph::setRoot(std::shared_ptr<Node> const &root){
    root_ = root;
};

std::shared_ptr<SceneGraph> SceneGraph::getInstance()
{
    singletonSceneGraph = std::make_shared<SceneGraph>();

    return singletonSceneGraph;
};

std::string SceneGraph::getName() const
{
    return name_;
};

std::shared_ptr<Node> SceneGraph::getRoot() const
{
    return root_;
};

std::string SceneGraph::printGraph() const
{
    return "hello";
}
