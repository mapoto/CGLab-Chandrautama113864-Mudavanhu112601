#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <string>
#include <Node.hpp>

class SceneGraph
{
private:
    static std::shared_ptr<SceneGraph> singletonSceneGraph;

    std::shared_ptr<Node> root_;
    std::string name_;

    void setName(std::string const &name);
    void setRoot(std::shared_ptr<Node> const &root);

    SceneGraph(std::string const &name);

public:
    static std::shared_ptr<SceneGraph> getInstance();
    std::string
    getName() const;
    std::shared_ptr<Node> getRoot() const;
    std::string printGraph() const;
};

#endif // SCENEGRAPH_HPP
