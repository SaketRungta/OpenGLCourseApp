#include "Light.h"

Light::Light()
{
	colour = glm::vec3(1.f, 1.f, 1.f);
	ambientIntensity = 1.f;

	direction = glm::vec3(0.f, -1.f, 0.f);
	diffuseIntensity = 0.f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity)
{
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;

	direction = glm::vec3(xDir, yDir, zDir);
	diffuseIntensity = dIntensity;
}

void Light::useLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f((GLint)ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f((GLint)ambientIntensityLocation, ambientIntensity);

	glUniform3f((GLint)directionLocation, direction.x, direction.y, direction.z);
	glUniform1f((GLint)diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light()
{
}
