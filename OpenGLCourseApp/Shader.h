#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

class Shader
{
public:
	Shader();

	void createFromString(const char* vertexCode, const char* fragmentCode);
	void createFromFiles(const char* vertexLocation, const char* fragmentLocation);
	std::string readFile(const char* fileLocation);
	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	void useShader();
	void clearShader();

	~Shader();

private:
	GLuint shaderProgram;
	GLuint uniformProjection;
	GLuint uniformModel;

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

