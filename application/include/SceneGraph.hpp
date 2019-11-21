#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <Node.hpp>
#include <string>

class SceneGraph {
public:
	static SceneGraph *getInstance(); 

private:
	SceneGraph(){}
	static SceneGraph* instance;
};

SceneGraph* SceneGraph::instance = nullptr;
SceneGraph* SceneGraph::getInstance() 
{
	if(!instance) {
		instance = new SceneGraph();
		std::cout << "getInstance(): First instance\n";
		return instance;
	}
	else {
		std::cout << "getInstance(): previous instance\n";
		return instance;
	}

};

#endif  // SCENEGRAPH_HPP
