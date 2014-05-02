#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normalMatrix;

smooth out vec3 worldPos;
smooth out vec3 diffuseColor;
smooth out vec3 normalWorld;

void main() {
	gl_Position = mvp * position;
	diffuseColor = vec3(0.6, 0.6, 0.6);
	normalWorld = normalize(normalMatrix * normal);
	worldPos = (model * position).xyz;
}