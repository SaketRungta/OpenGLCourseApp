#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint screenWidth = 1920;
const GLint screenHeight = 1080;

const float toRadians = 3.14159265f / 180.f;

GLuint VAO;
GLuint VBO;
GLuint IBO;
GLuint shaderProgram;
GLuint uniformModel;
GLuint uniformProjection;

bool direction = true;
float triOffset = 0.f;
float triMaxOffset = 1.f;
float triIncrement = 0.002f;

bool sizeDirectiion = true;
float currSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

float currAngle = 0.f;

static const char* vShader = "									\n\
#version 330													\n\
																\n\
layout (location = 0) in vec3 pos;								\n\
																\n\
out vec4 vCol;													\n\
																\n\
uniform mat4 model;												\n\
																\n\
uniform mat4 projection;										\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = projection * model * vec4(pos, 1.0);			\n\
	vCol = vec4(clamp(pos, 0.f, 1.f), 1.0);						\n\
}";

static const char* fShader = "									\n\
#version 330													\n\
																\n\
in vec4 vCol;													\n\
																\n\
out vec4 colour;												\n\
																\n\
void main()														\n\
{																\n\
	colour = vCol;												\n\
}";

void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("ERROR::addShader error compiling %d the shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void compileShader()
{
	shaderProgram = glCreateProgram();
	if (!shaderProgram)
	{
		printf("ERROR::compileShader error creating shader program\n");
		return;
	}

	addShader(shaderProgram, vShader, GL_VERTEX_SHADER);
	addShader(shaderProgram, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
		printf("ERROR::compileShader error linking shader program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
		printf("ERROR::compileShader error validating shader program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shaderProgram, "model");
	uniformProjection = glGetUniformLocation(shaderProgram, "projection");
}

void createTriangle()
{
	GLfloat vertices[] = {
		-1.f,  0.f,  1.f,
		 1.f,  0.f,  1.f,
		 1.f,  0.f, -1.f,
		-1.f,  0.f, -1.f,
		 0.f,  1.f,  0.f
	};

	unsigned int indices[] = {
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0,
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main()
{
	if (!glfwInit())
	{
		printf("ERROR::main GLFW Initialization FAILED");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Test Window", NULL, NULL);
	if (!window)
	{
		printf("ERROR::main GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;

	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(window);

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		printf("ERROR::main GLEW initialization failed");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	createTriangle();
	compileShader();

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferWidth, 0.1f, 100.f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(0.f, triOffset - 0.5f, -2.5f));
		model = glm::rotate(model, currAngle * toRadians, glm::vec3(0.f, 2.5f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 1.f, 0.4f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window);
	}

	return 0;
}