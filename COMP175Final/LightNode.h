#pragma once
#include "SceneNode.h"
#include "Mesh.h"
#include "CameraNode.h"
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class LightNode :
	public SceneNode
{
public:
	LightNode(float initialIntensity, float ir, float ig, float ib);
	~LightNode();
	virtual void onLoop(SceneManager* manager);
	void setMesh(Mesh* m);
private:
	Mesh* mesh;
	GLfloat intensity;
	float r;
	float g;
	float b;
	float radius;
};

