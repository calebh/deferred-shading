#pragma once

#include <GL/glew.h>
#include <iostream>

#ifndef NULL
#define NULL 0
#endif 

class GBuffer
{
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		//GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();

	void init(int width, int height);
	void startFrame();
	void bindForGeomPass();
	void bindForStencilPass();
	void bindForLightPass();
	void bindForFinalPass();
	void bindForBlitting();
	void setReadBuffer(GBUFFER_TEXTURE_TYPE textureType);
private:
	GLuint fbo;
	GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthTexture;
	GLuint finalTexture;
};

