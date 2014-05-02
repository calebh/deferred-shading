#pragma once

class CameraNode;
class ModelNode;
class LightNode;
class Device;
class ShaderProgram;

//#include "CameraNode.h"
//#include "ModelNode.h"
//#include "LightNode.h"
//#include "Device.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class SceneManager
{
public:
	SceneManager(Device* initDevice);
	Device* getDevice();
	ShaderProgram* getGeometryProgram();
	ShaderProgram* getPointLightProgram();
	void setGeometryProgram(ShaderProgram* prog);
	void setPointLightProgram(ShaderProgram* prog);
	void setDirLightProgram(ShaderProgram* prog);
	CameraNode* getCameraNode();
	~SceneManager();
	void addNode(CameraNode* node);
	void addNode(ModelNode* node);
	void addNode(LightNode* node);
	void drawAll();
private:
	std::vector<ModelNode*> modelNodes;
	std::vector<LightNode*> lightNodes;
	Device* device;
	CameraNode* camera;
	ShaderProgram* geometryProgram;
	ShaderProgram* pointLightProgram;
	ShaderProgram* dirLightProgram;
	Mesh sphere;
	Mesh quad;
	float diffuseConstant;
	float ambientConstant;
};