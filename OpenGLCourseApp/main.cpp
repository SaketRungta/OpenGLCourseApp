#include <stdio.h>

#include <GL/glew.h>
#include<GLFW/glfw3.h>

// Window Dimensions
const GLint WIDTH = 800, HEIGHT = 600;

int main() 
{
	// Initialise GLFW
	if (!glfwInit()) 
	{
		printf("GLFW Initialization failed");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	/*
	* GLFW_CONTEXT_VERSION_MAJOR = Tells that we are using version 3 of glfw
	* GLFW_CONTEXT_VERSION_MINOR = Tells that we are using version .3 of glfw
	* Combining it comes to version 3.3
	*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	/*
	* GLFW_OPENGL_CORE_PROFILE = Core profile means it wont be backwards compatible, we dont want to use depreceated features, features that were removed or are being planned to remove 
	* So if it detects any old openGL code, things that we should not be using because in future versions they will be removed completely, this will throw up an error
	*/
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// We have setup the properties of our GLFW window now we need to create a window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow) 
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the context for glew to use
	// We want glew to know that this is the window we have created here is the one opengl context should be tied to, so when everything gets drawn it should be drawn to this window
	// In short we are saying use the window we have just created and draw to that
	glfwMakeContextCurrent(mainWindow); // So if we have two window we can set context to that window

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	// Initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW Initialization failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Loop until the window is closed
	while (!glfwWindowShouldClose(mainWindow)) 
	{
		// Get + handle user input events
		// Checks if any events happened like keyboard presses, mouse movements, moving the window and resizing
		glfwPollEvents();

		// Clear the window
		glClearColor(1.f, 1.f, 0.f, 0.75f);
		glClear(GL_COLOR_BUFFER_BIT); // Clear the color data

		/* 
		* So we have these two scenes one which user is seeing right now and the above one in which we are drawing for the user to see next
		* Right now user has a scene in front of him, we have drawn an new scene through the above code, now we want to sawp the scene and make user see this scene
		*/
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}