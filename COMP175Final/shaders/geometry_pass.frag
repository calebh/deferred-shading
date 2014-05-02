#version 330

smooth in vec3 worldPos;
smooth in vec3 diffuseColor;
smooth in vec3 normalWorld;

layout (location = 0) out vec3 worldPosOut;
layout (location = 1) out vec3 diffuseOut;
layout (location = 2) out vec3 normalOut;

void main(void) {
	worldPosOut = worldPos;
	diffuseOut = diffuseColor;
	normalOut = normalWorld;
}