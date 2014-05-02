#include "LightNode.h"

const GLfloat dropRatio = 0.05f;

LightNode::LightNode(float initialIntensity, float ir, float ig, float ib) :
	intensity(initialIntensity),
	r(ir),
	g(ig),
	b(ib)
{
	radius = sqrtf(intensity / dropRatio);
	getTransform().setScale(radius, radius, radius);
}


LightNode::~LightNode()
{
}

void LightNode::setMesh(Mesh* m) {
	mesh = m;
}

void LightNode::onLoop(SceneManager* manager) {
	CameraNode* camera = manager->getCameraNode();
	ShaderProgram* shaderProgram = manager->getPointLightProgram();
	
	glm::mat4 mvp = camera->getProjection() * camera->getView() * getTransform().getTransformation();
	shaderProgram->setUniformMatrix4fv("mvp", 1, false, glm::value_ptr(mvp));
	shaderProgram->setUniform1f("lightIntensity", intensity);
	glm::vec3& lightPos = getTransform().getTranslation();
	shaderProgram->setUniform3f("lightPos", lightPos.x, lightPos.y, lightPos.z);
	shaderProgram->setUniform3f("lightColor", r, g, b);
	mesh->draw(manager->getGeometryProgram(), false);
}