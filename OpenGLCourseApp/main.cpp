#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

Window mainWindow;

std::vector<std::unique_ptr<Mesh>> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;

Light mainLight;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

const float toRadians = 3.14159265f / 180.f;

GLuint VAO;
GLuint VBO;
GLuint IBO;

bool direction = true;
float triOffset = 0.f;
float triMaxOffset = 1.f;
float triIncrement = 0.02f;

bool sizeDirectiion = true;
float currSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

float currAngle = 0.f;

static const char* vShader = "Shaders/shader.vert";

static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);

		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

void createObjects()
{
	GLfloat vertices[] = {
	//	  x		 y	    z		 u      v        nx     ny     nz
		-1.0f,  0.0f,  1.0f,	0.0f,  0.0f,	0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  1.0f,	1.0f,  0.0f,	0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f, -1.0f,	1.0f,  1.0f,	0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f, -1.0f,	0.0f,  1.0f,    0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,    0.5f,  1.0f,    0.0f,  0.0f,  0.0f
	};

	unsigned int indices[] = {
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0,
		0, 1, 2,
		0, 2, 3
	};

	calcAverageNormals(indices, 18, vertices, 40, 8, 5);

	meshList.push_back(std::make_unique<Mesh>(vertices, indices, 40, 18));
	meshList.push_back(std::make_unique<Mesh>(vertices, indices, 40, 18));
}

void createShaders()
{
	Shader* shader1 = new Shader();
	shader1->createFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window();
	mainWindow.initialise();

	createObjects();
	createShaders();

	camera = Camera(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f, 5.f, 0.1f);
	
	brickTexture = Texture((char*)"Textures/brick.png");
	brickTexture.loadTexture();

	dirtTexture = Texture((char*)"Textures/dirt.png");
	dirtTexture.loadTexture();
	
	mainLight = Light(1.f, 1.f, 1.f, 1.0f, 
					2.f, -1.f, 2.f, 1.f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformAmbientIntensity = 0;
	GLuint uniformAmbientColour = 0;
	GLuint uniformDiffuseIntensity = 0;
	GLuint uniformDirection = 0;

	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = (GLfloat)glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		/*
		{
			if (direction)
				triOffset += triIncrement;
			else
				triOffset -= triIncrement;

			if (abs(triOffset) >= triMaxOffset)
				direction = !direction;

			if (sizeDirectiion)
				currSize += 0.001f;
			else
				currSize -= 0.001f;

			if (currSize >= maxSize || currSize <= minSize)
				sizeDirectiion = !sizeDirectiion;

			currAngle += 0.1f;
			if (currAngle >= 360.f)
				currAngle -= 360.f;

		}
		*/

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectionLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformAmbientColour = shaderList[0].getAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].getAmbientIntensityLocation();
		uniformDiffuseIntensity = shaderList[0].getDiffuseIntensityLocation();
		uniformDirection = shaderList[0].getDirectionLocation();
		
		mainLight.useLight(uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection);

		glm::mat4 model(1.f);
		
		model = glm::translate(model, glm::vec3(0.f, 1.f, -5.f));
		model = glm::rotate(model, currAngle * toRadians, glm::vec3(0.f, 2.5f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 1.f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		brickTexture.useTexture();
		meshList[0]->renderMesh();

		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.f, -1.f, -5.f));
		model = glm::rotate(model, currAngle * toRadians, glm::vec3(0.f, 2.5f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 1.f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.useTexture();
		meshList[1]->renderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}