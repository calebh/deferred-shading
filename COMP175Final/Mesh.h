#pragma once

#include <GL/glew.h>
#include "ShaderProgram.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh
{
public:
	Mesh(const std::string& filename);
	~Mesh();
	void draw(ShaderProgram* shader, bool drawNormals);
private:
	GLuint positionBufferObject;
	GLuint indexBufferObject;
	GLuint normalBufferObject;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
};

