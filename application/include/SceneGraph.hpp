#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <Node.hpp>
#include <string>

class SceneGraph {
 private:
  static SceneGraph* singletonSceneGraph;

  std::shared_ptr<Node> root_;
  std::string name_;

  void setName(std::string const& name);
  void setRoot(std::shared_ptr<Node> const& root);

  SceneGraph(std::string const& name);

 public:
  static SceneGraph* getInstance(std::string const& name);
  std::string getName() const;
  std::shared_ptr<Node> getRoot() const;
  std::string printGraph() const;
};

#endif  // SCENEGRAPH_HPP
