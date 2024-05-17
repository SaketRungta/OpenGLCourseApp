#pragma once

#include<GL\glew.h>
#include "stb_image.h"

class Texture
{
public:
	Texture();
	Texture(char *fileLoc);
	
	void loadTexture();
	void useTexture();
	void clearTexture();

	~Texture();

private:
	GLuint textureID;
	int width;
	int height;
	int bitDepth;

	char* fileLocation;
};