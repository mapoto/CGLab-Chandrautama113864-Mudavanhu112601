#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <memory>
#include <string>

//include all useful nodes
#include "CameraNode.hpp"
#include "GeometryNode.hpp"
#include "PointLightNode.hpp"

/*

The SceneGraph helps us to keep track of all our dependences and structure of the universe 
The SceneGraph is Created only once and acts as our tree where we have a root Node and 
all the other Nodes (Children Created) will branch from this main 'ancenstor Node'
*/

/////////////////////////////////////////////////////////////////////////////

class SceneGraph {
 public:

  //SceneGraph default constructor 
  SceneGraph();

  //SceneGraph desctructor 
  ~SceneGraph();

  // Setters used to give the Scene a Name and setting the Root Node
  void setName(std::string const& name);
  void setRoot(Node* root);


  //Getters for getting the Scene Name, the RootNode
  std::string getName() const;
  Node* getRoot() const;

  //Priniting the Name of the Scene
  std::string printGraph() const;

 private:
  Node* root_;
  std::string name_;
  //static SceneGraph* instance;
};

#endif  // SCENEGRAPH_HPP
