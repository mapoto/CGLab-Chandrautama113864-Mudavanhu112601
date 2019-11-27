#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <Node.hpp>

//////////////////////////////////////////////////////////////////////

class CameraNode : public Node {
 private:
  bool isPerspective_;
  bool isEnabled_;
  glm::mat4 projectionMatrix_;

 public:
  //User Defined Constructor 
  CameraNode(std::string const& name);

  //Destructor 
  ~CameraNode();

  //Getter and Setter functions for the Perspective of the CameraNode
  bool getPerspective() const;
  void setPerspective(bool value);

  //Getter and Setter functions for the Enable of the CameraNode
  bool getEnabled() const;
  void setEnabled(bool value);

  //Getter and Setter functions for the projectionMatrix of the CameraNode
  glm::mat4 getProjectionMatrix() const;
  void setProjectionMatrix(glm::mat4 const& projection);
};

#endif  // CAMERA_NODE_HPP