#include <CameraNode.hpp>


////////////////////////////////////////////////////////////////////////////////////////
/*
The CameraNode allows us to create Node that we can use as our viewing point in relation to 
all the created nodes in the Scene Graph.

*/

//////////////////////////////////////////////////////////////////////////////////////
//User defined Constructor of the CameraNode that passes a Name to the CameraNode

CameraNode::CameraNode(std::string const& name)
    : Node{name},
      isPerspective_{true},
      isEnabled_{true},
      projectionMatrix_{1} {}

//Destructor 
CameraNode::~CameraNode(){
}

///////////////////////////////////////////////////////////////////////////////////////
//Function that checks if the isPerspective of the CameraNode is avaliable or not

bool CameraNode::getPerspective() const {
  return isPerspective_;
}

//////////////////////////////////////////////////////////////////////////////////////
//Function Call that sets the Perspective value of the CameraNode

void CameraNode::setPerspective(bool value) {
  isPerspective_ = value;
}

/////////////////////////////////////////////////////////////////////////////////////
//The function call that gets the isEnabled valuefor the Perspective of the CameraNode

bool CameraNode::getEnabled() const {
  return isEnabled_;
}


//////////////////////////////////////////////////////////////////////////////////////
//The call is used to Enable the Valued of the CameraNode Perspective to be turned on 

void CameraNode::setEnabled(bool value) {
  isEnabled_ = value;
}

/////////////////////////////////////////////////////////////////////////////////////
//The Function that geths the projectionMatrix_ that is used for giving the Perspective
//Of the CameraNode with Relation to its position  

glm::mat4 CameraNode::getProjectionMatrix() const {
  return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(glm::mat4 const& projection) {
  projectionMatrix_ = projection;
}
