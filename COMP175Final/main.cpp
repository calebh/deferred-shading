#define GLFW_INCLUDE_GL_3
#include <GL/glew.h>
#include "Device.h"
#include "Transform.h"
#include "SceneManager.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "ModelNode.h"
#include "CameraNode.h"
#include "LightNode.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

float randFloat() {
	return rand() / ((float)RAND_MAX);
}

int main(void) {
	int width = 1366;
	int height = 768;

	Device device(width, height, true);
	CameraNode camera(width, height);

	camera.getTransform().setTranslation(20.0f, 20.0f, 20.0f);

	ShaderProgram geometryProgram;
	Shader geometryVertexShader("shaders/geometry_pass.vert", VERTEX);
	Shader geometryFragShader("shaders/geometry_pass.frag", FRAGMENT);
	geometryProgram.attachShader(&geometryVertexShader);
	geometryProgram.attachShader(&geometryFragShader);
	geometryProgram.link();

	ShaderProgram pointLightProgram;
	Shader lightPassVertexShader("shaders/point_light_pass.vert", VERTEX);
	Shader pointLightFragShader("shaders/point_light_pass.frag", FRAGMENT);
	pointLightProgram.attachShader(&lightPassVertexShader);
	pointLightProgram.attachShader(&pointLightFragShader);
	pointLightProgram.link();

	ShaderProgram dirLightProgram;
	Shader dirLightPassVertexShader("shaders/dir_light_pass.vert", VERTEX);
	Shader dirLightFragShader("shaders/dir_light_pass.frag", FRAGMENT);
	Shader dirLightGeomShader("shaders/dir_light_pass.geom", GEOMETRY);
	dirLightProgram.attachShader(&dirLightPassVertexShader);
	dirLightProgram.attachShader(&dirLightFragShader);
	dirLightProgram.attachShader(&dirLightGeomShader);
	dirLightProgram.link();

	ShaderProgram stencilProgram;
	Shader stencilVertexShader("shaders/stencil_pass.vert", VERTEX);
	Shader stencilFragShader("shaders/stencil_pass.frag", FRAGMENT);
	stencilProgram.attachShader(&stencilVertexShader);
	stencilProgram.attachShader(&stencilFragShader);
	stencilProgram.link();

	SceneManager manager(&device);
	manager.setGeometryProgram(&geometryProgram);
	manager.setPointLightProgram(&pointLightProgram);
	manager.setDirLightProgram(&dirLightProgram);
	manager.setStencilProgram(&stencilProgram);

	//LightNode light(100.0f, 1.0f, 0.0f, 0.0f);
	//light.getTransform().setTranslation(-5.0f, -5.0f, -5.0f);

	//LightNode light2(100.0f, 0.0f, 0.0f, 1.0f);
	//light.getTransform().setTranslation(5.0f, 5.0f, 5.0f);

	manager.addNode(&camera);
	//manager.addNode(&light);
	//manager.addNode(&light2);

	Mesh cube("models/cube.obj");
	ModelNode base;
	base.setMesh(&cube);
	base.getTransform().setScale(100.0f, 0.25f, 100.0f);
	manager.addNode(&base);

	Mesh helicoptor("models/jeep.obj");

	//int range = 90;
	int range = 60;

	std::vector<LightNode*> allLights;

	for (int x = -range; x <= range; x += 20) {
		for (int z = -range; z <= range; z += 20) {
			float xf = (float)x;
			float zf = (float)z;

			// WARNING WARNING MEMORY LEAK
			ModelNode* mn = new ModelNode();
			LightNode* light = new LightNode(40.0f, randFloat(), randFloat(), randFloat());
			light->direction = glm::vec3((randFloat() * 2.0f) - 1.0f, 0.0f, (randFloat() * 2.0f) - 1.0f);
			allLights.push_back(light);
			light->getTransform().setTranslation(xf - 1.0f, 7.0f, zf);
			//light->getTransform().setTranslation(xf + 5.0f, 0.5f, zf + 5.0f);
			//light->getTransform().setTranslation(xf + (randFloat() - 0.5f) * 3.0f, randFloat() * 2.0f + 7.0f, zf + (randFloat() - 0.5f) * 3.0f);

			manager.addNode(light);

			mn->getTransform().setScale(0.025f, 0.025f, 0.025f);
			mn->getTransform().setTranslation(xf, 0.0f, zf);
			mn->setMesh(&helicoptor);
			manager.addNode(mn);
		}
	}

	bool autoDemo = false;

	int prevKeyState = GLFW_RELEASE;

	int t = 0;
	while (device.run()) {
		int keyState = glfwGetKey(device.getWindow(), GLFW_KEY_T);
		if (keyState == GLFW_RELEASE && prevKeyState == GLFW_PRESS) {
			autoDemo = !autoDemo;
		}
		prevKeyState = keyState;

		if (autoDemo) {
			if (t > 600 && t < 900) {
				manager.setDebugGBuffer(true);
			}
			else if (t >= 900) {
				manager.setDebugGBuffer(false);
				t = 0;
			}
		}

		if (t == 300) {
			for (LightNode* n : allLights) {
				n->setRGB(randFloat(), randFloat(), randFloat());
			}
		}

		for (LightNode* n : allLights) {
			glm::vec3 currPos = n->getTransform().getTranslation();
			currPos += n->direction * 0.1f;
			if (currPos.x > 70) {
				currPos.x = -70;
			}
			if (currPos.x < -70) {
				currPos.x = 70;
			}

			if (currPos.z > 70) {
				currPos.z = -70;
			}
			if (currPos.z < -70) {
				currPos.z = 70;
			}
			n->getTransform().setTranslation(currPos);
		}

		//camera.getTransform().setTranslation(200.0f, 200.0f, 200.0f);
		if (glfwGetKey(device.getWindow(), GLFW_KEY_W)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + camera.getLookVector());
		} else if (glfwGetKey(device.getWindow(), GLFW_KEY_S)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + camera.getLookVector() * -1.0f);
		}

		glm::vec3 horizontal = glm::normalize(glm::cross(camera.getUp(), camera.getLookVector()));
		if (glfwGetKey(device.getWindow(), GLFW_KEY_A) || autoDemo) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + horizontal);
		} else if (glfwGetKey(device.getWindow(), GLFW_KEY_D)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + horizontal * -1.0f);
		}

		t++;
		manager.drawAll();
		device.endScene();
	}
}