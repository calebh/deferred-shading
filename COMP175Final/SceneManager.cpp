#include "SceneManager.h"
#include "CameraNode.h"
#include "ModelNode.h"
#include "LightNode.h"
#include "Device.h"
#include "ShaderProgram.h"

SceneManager::SceneManager(Device* initDevice) :
	device(initDevice),
	geometryProgram(NULL),
	pointLightProgram(NULL),
	dirLightProgram(NULL),
	stencilProgram(NULL),
	camera(NULL),
	sphere("models/sphere.obj"),
	diffuseConstant(0.7f),
	ambientConstant(0.1f),
	directionalIntensity(0.2f),
	debugGBuffer(false)
{
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
}


SceneManager::~SceneManager()
{
}

Device* SceneManager::getDevice() {
	return device;
}

void SceneManager::setGeometryProgram(ShaderProgram* prog) { geometryProgram = prog; }
void SceneManager::setPointLightProgram(ShaderProgram* prog) { pointLightProgram = prog; }
void SceneManager::setDirLightProgram(ShaderProgram* prog) { dirLightProgram = prog; }
void SceneManager::setStencilProgram(ShaderProgram* prog) { stencilProgram = prog; }

ShaderProgram* SceneManager::getGeometryProgram() { return geometryProgram; }
ShaderProgram* SceneManager::getPointLightProgram() { return pointLightProgram; }
ShaderProgram* SceneManager::getDirLightProgram() { return dirLightProgram; }
ShaderProgram* SceneManager::getStencilProgram() { return stencilProgram; }

CameraNode* SceneManager::getCameraNode() {
	return camera;
}

void SceneManager::setDebugGBuffer(bool d) {
	debugGBuffer = d;
}

void SceneManager::addNode(CameraNode* node) {
	camera = node;
}

void SceneManager::addNode(ModelNode* node) {
	modelNodes.push_back(node);
}

void SceneManager::addNode(LightNode* node) {
	node->setMesh(&sphere);
	lightNodes.push_back(node);
}

void SceneManager::geometryPass() {
	GBuffer& gbuffer = device->getGbuffer();
	geometryProgram->use();
	gbuffer.bindForGeomPass();
	
	// Only the geometry pass updates the depth buffer
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	for (ModelNode* m : modelNodes) {
		m->geometryPass(this);
	}

	// Lock down the depth buffer
	glDepthMask(GL_FALSE);
}

void SceneManager::stencilPass(LightNode* light) {
	GBuffer& gbuffer = device->getGbuffer();
	gbuffer.bindForStencilPass();
	stencilProgram->use();
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 0, 0);
	//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	light->stencilPass(this);
}

void SceneManager::pointLightPass(LightNode* light) {
	GBuffer& gbuffer = device->getGbuffer();
	gbuffer.bindForLightPass();

	pointLightProgram->use();

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	float width = (float)device->getWidth();
	float height = (float)device->getHeight();
	pointLightProgram->setUniform2f("screenSize", width, height);
	pointLightProgram->setUniform1f("diffuseConstant", diffuseConstant);

	pointLightProgram->setUniform1i("positionMap", 0);
	pointLightProgram->setUniform1i("colorMap", 1);
	pointLightProgram->setUniform1i("normalMap", 2);

	light->pointLightPass(this);

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void SceneManager::directionalLightPass() {
	float width = (float)device->getWidth();
	float height = (float)device->getHeight();
	GBuffer& gbuffer = device->getGbuffer();

	dirLightProgram->use();
	gbuffer.bindForLightPass();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);

	// Bind shit for the directional light shader
	dirLightProgram->setUniform1i("colorMap", 1);
	dirLightProgram->setUniform1i("normalMap", 2);

	dirLightProgram->setUniform3f("lightDir", -1.0f, -1.0f, -1.0f);
	dirLightProgram->setUniform1f("lightIntensity", directionalIntensity);
	dirLightProgram->setUniform1f("diffuseConstant", diffuseConstant);
	dirLightProgram->setUniform2f("screenSize", width, height);
	dirLightProgram->setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	dirLightProgram->setUniform1f("ambientConstant", ambientConstant);

	// Geometry shader actually makes the quad
	glDrawArrays(GL_POINTS, 0, 1);

	glDisable(GL_BLEND);
}

void SceneManager::finalPass() {
	device->getGbuffer().bindForFinalPass();
	int width = device->getWidth();
	int height = device->getHeight();
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void SceneManager::drawAll() {
	GBuffer& gbuffer = device->getGbuffer();

	// Update the camera matrices
	camera->updateView();
	gbuffer.startFrame();
	
	geometryPass();

	if (glfwGetKey(device->getWindow(), GLFW_KEY_B) == GLFW_PRESS || debugGBuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		device->getGbuffer().bindForBlitting();
		int windowWidth = device->getWidth();
		int windowHeight = device->getHeight();

		GLsizei HalfWidth = (GLsizei)(windowWidth / 2.0f);
		GLsizei HalfHeight = (GLsizei)(windowHeight / 2.0f);

		// Lower left
		gbuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
		glBlitFramebuffer(0, 0, windowWidth, windowHeight,
			0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Upper left
		gbuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
		glBlitFramebuffer(0, 0, windowWidth, windowHeight,
			0, HalfHeight, HalfWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Upper right
		gbuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
		glBlitFramebuffer(0, 0, windowWidth, windowHeight,
			HalfWidth, HalfHeight, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	} else {
		glEnable(GL_STENCIL_TEST);
		for (LightNode* light : lightNodes) {
			stencilPass(light);
			pointLightPass(light);
		}
		glDisable(GL_STENCIL_TEST);

		directionalLightPass();
		finalPass();
	}
}