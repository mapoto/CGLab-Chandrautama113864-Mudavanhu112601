#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <Node.hpp>

class CameraNode : public Node {
 private:
  bool isPerspective_;
  bool isEnabled_;
  glm::mat4 projectionMatrix_;

 public:
  CameraNode(std::string const& name, std::string const& path, int depth);
  CameraNode(std::string const& name);

  ~CameraNode();

  bool getPerspective() const;
  void setPerspective(bool value);

  bool getEnabled() const;
  void setEnabled(bool value);

  glm::mat4 getProjectionMatrix() const;
  void setProjectionMatrix(glm::mat4 const& projection);
};

#endif  // CAMERA_NODE_HPP