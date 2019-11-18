#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <model.hpp>
#include <Node.hpp>

class GeometryNode : public Node
{
private:
    model geometry_;

public:
    GeometryNode(std::string const &name, std::string const &path, std::int32_t &depth, model const &geometry_model);
    GeometryNode(std::string const &name, model const &geometry_model);

    ~GeometryNode();

    model getGeometry();
    void setGeometry(model const &geometry_model);
};

#endif // GEOMETRY_HPP