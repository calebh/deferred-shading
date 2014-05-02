#pragma once

#include "Transform.h"
#include "Device.h"
#include "SceneManager.h"
#include "Mesh.h"

class SceneNode
{
public:
	SceneNode();
	~SceneNode();
	Transform& getTransform();
	virtual void onLoop(SceneManager* manager);

private:
	Transform transform;
	Mesh* mesh;
};