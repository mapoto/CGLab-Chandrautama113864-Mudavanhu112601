#include <CameraNode.hpp>

CameraNode::CameraNode(std::string const& name)
    : Node{name},
      isPerspective_{true},
      isEnabled_{true},
      projectionMatrix_{1} {};

bool CameraNode::getPerspective() const {
  return isPerspective_;
};

void CameraNode::setPerspective(bool value) {
  isPerspective_ = value;
};

bool CameraNode::getEnabled() const {
  return isEnabled_;
};

void CameraNode::setEnabled(bool value) {
  isEnabled_ = value;
};

glm::mat4 CameraNode::getProjectionMatrix() const {
  return projectionMatrix_;
};
void CameraNode::setProjectionMatrix(glm::mat4 const& projection) {
  projectionMatrix_ = projection;
};
