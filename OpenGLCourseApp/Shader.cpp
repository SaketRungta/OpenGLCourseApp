#include "Shader.h"

Shader::Shader()
{
	shaderProgram = 0;
	uniformProjection = 0;
	uniformModel = 0;
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode)
{
	compileShader(vertexCode, fragmentCode);
}

void Shader::createFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = readFile(vertexLocation);
	std::string fragmentString = readFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	compileShader(vertexCode, fragmentCode);
}

std::string Shader::readFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		printf("ERROR::Shader::readFile failed to read %s", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

void Shader::useShader()
{
	glUseProgram(shaderProgram);
}

void Shader::clearShader()
{
	if (shaderProgram != 0)
	{
		glDeleteProgram(shaderProgram);
		shaderProgram = 0;
	}

	uniformProjection = 0;
	uniformModel = 0;
}

Shader::~Shader()
{
	clearShader();
}

void Shader::compileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderProgram = glCreateProgram();
	if (!shaderProgram)
	{
		printf("ERROR::compileShader error creating shader program\n");
		return;
	}

	addShader(shaderProgram, vertexCode, GL_VERTEX_SHADER);
	addShader(shaderProgram, fragmentCode, GL_FRAGMENT_SHADER);

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

void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
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
