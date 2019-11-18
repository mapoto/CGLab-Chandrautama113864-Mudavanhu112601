#include <Node.hpp>

class CameraNode: public Node
{
private:
    bool isPerspective_;
    bool isEnabled_;
    glm::mat4 projectionMatrix_;

public:
    CameraNode(std::string const &name, std::string const &path, std::int32_t &depth);
    CameraNode(std::string const &name);
    
    ~CameraNode();

    bool getPerspective();
    void setPerspective(bool &value);

    bool getEnabled();
    void setEnabled(bool &value);

    glm::mat4 getProjectionMatrix();
    void setProjectionMatrix(glm::mat4 projection);

};