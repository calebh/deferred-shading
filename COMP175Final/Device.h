#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "GBuffer.h"

class Device
{
public:
	Device(int initialWidth, int initialHeight, bool fullscreen);
	~Device();
	GLFWwindow* getWindow();
	bool run();
	int getWidth();
	int getHeight();
	void endScene();
	GBuffer& getGbuffer();
private:
	GLFWwindow* window;
	GBuffer gbuffer;
	int width;
	int height;
};

