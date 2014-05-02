#include "Mesh.h"

Mesh::Mesh(const std::string& filename) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_GenNormals |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene) {
		std::cerr << "Scene is null";
	}

	if (scene->HasMeshes()) {
		aiMesh* m = scene->mMeshes[0];
		for (unsigned int i = 0; i < m->mNumVertices; i++) {
			aiVector3D* vertex = &(m->mVertices[i]);
			aiVector3D* normal = &(m->mNormals[i]);
			vertices.push_back(vertex->x);
			vertices.push_back(vertex->y);
			vertices.push_back(vertex->z);
			normals.push_back(normal->x);
			normals.push_back(normal->y);
			normals.push_back(normal->z);			
		}
		for (unsigned int i = 0; i < m->mNumFaces; i++) {
			aiFace* face = &(m->mFaces[i]);
			if (face->mNumIndices != 3) {
				std::cerr << "numIndices is not 3";
			}
			for (unsigned int j = 0; j < face->mNumIndices; j++) {
				indices.push_back(face->mIndices[j]);
			}
		}
	} else {
		std::cerr << "Scene has no meshes";
	}

	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &(vertices[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &normalBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &(normals[0]), GL_STATIC_DRAW);
	
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* indices.size(), &(indices[0]), GL_STATIC_DRAW);
}


Mesh::~Mesh()
{
}

void Mesh::draw(ShaderProgram* shader, bool drawNormals) {
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	shader->vertexAttribPointer("position", 3, GL_FLOAT, 0, 0, GL_FALSE);

	if (drawNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
		shader->vertexAttribPointer("normal", 3, GL_FLOAT, 0, 0, GL_FALSE);
	}

	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
