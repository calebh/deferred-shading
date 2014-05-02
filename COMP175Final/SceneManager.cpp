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
	camera(NULL),
	sphere("models/sphere.obj"),
	quad("models/quad.obj"),
	diffuseConstant(0.7f),
	ambientConstant(0.3f)
{
	
}


SceneManager::~SceneManager()
{
}

Device* SceneManager::getDevice() {
	return device;
}

void SceneManager::setGeometryProgram(ShaderProgram* prog) {
	geometryProgram = prog;
}

void SceneManager::setPointLightProgram(ShaderProgram* prog) {
	pointLightProgram = prog;
}

void SceneManager::setDirLightProgram(ShaderProgram* prog) {
	dirLightProgram = prog;
}

ShaderProgram* SceneManager::getGeometryProgram() {
	return geometryProgram;
}

ShaderProgram* SceneManager::getPointLightProgram() {
	return pointLightProgram;
}

CameraNode* SceneManager::getCameraNode() {
	return camera;
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

void SceneManager::drawAll() {
	GBuffer& gbuffer = device->getGbuffer();

	glCullFace(GL_TRUE);

	// Do the geometry pass
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	gbuffer.bindForWriting();
	geometryProgram->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	// Update camera matrices
	if (camera) {
		camera->onLoop(this);
	}
	
	for (ModelNode* m : modelNodes) {
		m->onLoop(this);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glfwGetKey(device->getWindow(), GLFW_KEY_B) == GLFW_PRESS) {
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
		// Do the light pass

		// When we get here the depth buffer is already populated and the stencil pass
		// depends on it, but it does not write to it.
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		pointLightProgram->use();

		pointLightProgram->setUniform1i("positionMap", 0);
		pointLightProgram->setUniform1i("colorMap", 1);
		pointLightProgram->setUniform1i("normalMap", 2);

		gbuffer.bindForReading();
		glClear(GL_COLOR_BUFFER_BIT);

		float width = (float)device->getWidth();
		float height = (float)device->getHeight();
		for (LightNode* light : lightNodes) {
			pointLightProgram->setUniform2f("screenSize", width, height);
			pointLightProgram->setUniform1f("diffuseConstant", diffuseConstant);
			light->onLoop(this);
		}

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		dirLightProgram->use();
		
		dirLightProgram->setUniform1i("colorMap", 1);
		dirLightProgram->setUniform1i("normalMap", 2);

		gbuffer.bindForReading();
		
		dirLightProgram->setUniform3f("lightDir", -1.0f, -1.0f, -1.0f);
		dirLightProgram->setUniform1f("lightIntensity", 1.0f);
		dirLightProgram->setUniform1f("diffuseConstant", diffuseConstant);
		dirLightProgram->setUniform2f("screenSize", width, height);
		dirLightProgram->setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
		dirLightProgram->setUniform1f("ambientConstant", ambientConstant);

		//glCullFace(GL_FALSE);

		glDrawArrays(GL_POINTS, 0, 1);
	}
}