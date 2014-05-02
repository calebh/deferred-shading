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

int main(void) {
	int width = 640;
	int height = 480;

	Device device(width, height);
	CameraNode camera(width, height);

	camera.getTransform().setTranslation(20.0f, 20.0f, 20.0f);

	ShaderProgram geometryProgram;
	Shader geometryVertexShader("shaders/geometry_pass.vert", VERTEX);
	Shader geometryFragShader("shaders/geometry_pass.frag", FRAGMENT);
	geometryProgram.attachShader(&geometryVertexShader);
	geometryProgram.attachShader(&geometryFragShader);
	geometryProgram.link();

	ShaderProgram pointLightProgram;
	Shader lightPassVertexShader("shaders/light_pass.vert", VERTEX);
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

	SceneManager manager(&device);
	manager.setGeometryProgram(&geometryProgram);
	manager.setPointLightProgram(&pointLightProgram);
	manager.setDirLightProgram(&dirLightProgram);

	LightNode light(50.0f, 1.0f, 0.0f, 0.0f);
	light.getTransform().setTranslation(-5.0f, -5.0f, -5.0f);

	manager.addNode(&camera);
	manager.addNode(&light);

	Mesh helicoptor("models/hheli.obj");
	ModelNode mn;
	mn.getTransform().setScale(0.05f, 0.05f, 0.05f);
	mn.setMesh(&helicoptor);

	manager.addNode(&mn);

	float s = 0.0f;
	while (device.run()) {
		mn.getTransform().setRotation(sinf(s) * 3.14159f, 0.0f, 0.0f);

		//camera.getTransform().setTranslation(200.0f, 200.0f, 200.0f);
		if (glfwGetKey(device.getWindow(), GLFW_KEY_W)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + camera.getLookVector());
		} else if (glfwGetKey(device.getWindow(), GLFW_KEY_S)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + camera.getLookVector() * -1.0f);
		}

		glm::vec3 horizontal = glm::normalize(glm::cross(camera.getUp(), camera.getLookVector()));
		if (glfwGetKey(device.getWindow(), GLFW_KEY_A)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + horizontal);
		} else if (glfwGetKey(device.getWindow(), GLFW_KEY_D)) {
			camera.getTransform().setTranslation(camera.getTransform().getTranslation() + horizontal * -1.0f);
		}

		s += 0.001f;

		manager.drawAll();
		device.endScene();
	}
}