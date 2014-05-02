#include "GBuffer.h"


GBuffer::GBuffer() :
	fbo(0)
{

}

void GBuffer::init(int width, int height) {
	// Create the FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glGenTextures(GBUFFER_NUM_TEXTURES, textures);
	glGenTextures(1, &depthTexture);
	glGenTextures(1, &finalTexture);

	// Attach textures to FBO
	for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}

	// Attach depth texture NOW WITH MORE STENCIL TEXTURES
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	// Final texture is now in GBuffer because we need access to the stencil buffer
	glBindTexture(GL_TEXTURE_2D, finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, finalTexture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FB error, status: " << status << std::endl;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer() {
	if (fbo != 0) {
		glDeleteFramebuffers(1, &fbo);
	}

	if (textures[0] != 0) {
		glDeleteTextures(GBUFFER_NUM_TEXTURES, textures);
	}

	if (depthTexture != 0) {
		glDeleteTextures(1, &depthTexture);
	}
}

void GBuffer::startFrame() {
	// Clear the final buffer at the start of the frame
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::bindForGeomPass() {
	// Build initial GBuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	GLenum drawBuffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	// Don't touch the final buffer yet
	glDrawBuffers(3, drawBuffers);
}

void GBuffer::bindForStencilPass() {
	glDrawBuffer(GL_NONE);
}

void GBuffer::bindForLightPass() {
	// Draw to the final buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
	glActiveTexture(GL_TEXTURE0);
}

void GBuffer::bindForFinalPass() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT4);	
}

void GBuffer::bindForBlitting() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType) {
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}