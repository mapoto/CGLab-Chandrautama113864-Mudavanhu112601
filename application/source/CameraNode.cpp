#include <CameraNode.hpp>

CameraNode::CameraNode(std::string const &name,
                       std::string const &path,
                       std::int32_t &depth) : Node{name,
                                                   path,
                                                   depth},
                                              isPerspective_{true},
                                              isEnabled_{true},
                                              projectionMatrix_{1} {};
CameraNode::CameraNode(std::string const &name) : Node{name}, isPerspective_{true}, isEnabled_{true}, projectionMatrix_{1} {};

bool CameraNode::getPerspective()
{
    return isPerspective_;
};

void CameraNode::setPerspective(bool &value)
{
    isPerspective_ = value;
};

bool CameraNode::getEnabled()
{
    return isEnabled_;
};

void CameraNode::setEnabled(bool &value)
{
    isEnabled_ = value;
};

glm::mat4 CameraNode::getProjectionMatrix()
{
    return projectionMatrix_;
};
void CameraNode::setProjectionMatrix(glm::mat4 projection)
{
    projectionMatrix_ = projection;
};
