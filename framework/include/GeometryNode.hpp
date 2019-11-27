#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <Node.hpp>
#include <model.hpp>

///////////////////////////////////////////////////////////////////

class GeometryNode : public Node {
 private:
  model geometry_;

 public:
  //User Defined Constructor of the GeometryNode
  GeometryNode(std::string const& name, model const& geometry_model);

  //Destructor of the GeometryNode
  ~GeometryNode();

  //Getter and Setter Functions for the GeometryNode
  model getGeometry() const;
  void setGeometry(model const& geometry_model);
};

#endif  // GEOMETRY_NODE_HPP