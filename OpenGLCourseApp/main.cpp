#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window Dimensions
const GLint WIDTH = 800, HEIGHT = 800;

GLuint VAO, VBO, shaderProgram, uniformXMove;

bool direction = false;
float triOffset = 0.f, triMaxOffset = 0.5f, triIncrement = 0.002f;

// Vertex shader
// layout (location = 0) in vec3 pos; here location zero represents the lower attribute pointer location
static const char* vShader = "									\n\
#version 330													\n\
																\n\
layout (location = 0) in vec3 pos;								\n\
																\n\
uniform float xMove;																\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = vec4(0.4 * pos.x + xMove, 0.4 * pos.y - xMove, 0.4 * pos.z, 1.0);				\n\
}";

// Fragment shader
static const char* fShader = "									\n\
#version 330													\n\
																\n\
out vec4 colour;												\n\
																\n\
void main()														\n\
{																\n\
	colour = vec4(1.f, 0.f, 0.f, 1.0);							\n\
}";

/*
* Creates the requested shader and adds it our shader program
* @param theProgram: Shader program to add the shader to
* @param shaderCode: Code of the shader we trying to add
* @param shaderType: Type of shader we are creating
*/
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	// Creates an empty shader object an returns an non zero calue by which it can be referenced
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	/*
	* Any source code previously stored in the shader object is completely replaced
	* @param 1 Shader: Specifies the handle of the shader object whose source code is to be replaced.
	* @param 2 count: Specifies the number of elements in the string and length arrays.
	* @param 3 string: Specifies an array of pointers to strings containing the source code to be loaded into the shader.
	* @param 4 length: Specifies the length of the shader code being used.
	*/
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	// Result of the compilation
	GLint result = 0;
	// Error log if any
	GLchar eLog[1024] = { 0 };

	/*
	* Function to get information on the shader complie status
	* @param 1: The shader to get info about
	* @param 2: Which info we want to get about that program
	* @param 3: Result of the info
	*/
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	/*
	* In order to create a complete shader program, there must be a way to specify the list of things that will be linked together
	* Program object provides this mechanism
	* So shaders that are to be linked to a program object must be first attached to the program itself
	* Basically we are saying to attach the shader we just created to our shader program
	*/
	glAttachShader(theProgram, theShader);
}

void CompileShader()
{
	/*
	* Creates an empty program object and returns a non zero value by which it can be referenced
	* A program object is an object to which shaders objects can be attached
	* This provides a mechanism to specify the shader objects that will be linked to a program
	*/
	shaderProgram = glCreateProgram();

	// Make sure the shader was created correctly
	if (!shaderProgram)
	{
		printf("Error creating shader program\n");
		return;
	}

	AddShader(shaderProgram, vShader, GL_VERTEX_SHADER);
	AddShader(shaderProgram, fShader, GL_FRAGMENT_SHADER);

	// Result of the two shaders
	GLint result = 0;
	// Error log if any
	GLchar eLog[1024] = { 0 };

	/*
	* It will create executeables on the graphics card, its linking all program, together and making sure its all there and working
	* We have finished making the program but we need to check if the program linked properly and then we have to validate if its going to be working for the settings for openGL we have setup
	*/
	glLinkProgram(shaderProgram);

	/*
	* Function to get information on the program here
	* @param 1: Shader program to get info about
	* @param 2: Which info we want to get about the passed shader program
	* @param 3: Result of the info
	*/
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
		printf("Error linking shader program: '%s'\n", eLog);
		return;
	}

	// Checks to see whether the executables contained in the program can execute given the current openGL state 
	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
		printf("Error validating shader program: '%s'\n", eLog);
		return;
	}

	uniformXMove = glGetUniformLocation(shaderProgram, "xMove");
}

void CreateTraiangle()
{
	/*
	* The center of the window is 0,0
	* Y axis is stright up, X right, Z is depth
	* Left most point in the middle of screen is (-1,0) right most (1,0)
	* Top most point in the middle of screen is (0,1) bottom most (0,-1)
	*/
	GLfloat vertices[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
	};

	/*
	* Creating our VAO
	* @param 1: The amount of arrays we want to create
	* @param 2: Where we want to store the ID of the array
	* This function is now defining some space in the memory for one vertex array and then it's giving you the ID
	* When we want to access that VAO, we pass this ID
	*/
	glGenVertexArrays(1, &VAO);

	/*
	* We need to bind this VAO to get working on it
	* Now any OpenGL functions we use which interact with vertex arrays or buffers they all will be taking place within this vertex defined by that VAO ID
	*/
	glBindVertexArray(VAO);

	/*
	* Now we have our VAO we need to create a buffer object to go inside that VAO
	* Works similarily to that VAO
	*/
	glGenBuffers(1, &VBO);

	/*
	* Binding it similar to that of VAO
	* @param 1: We want to enter which buffer we want to bind it to as we can bind VBO to multiple buffers
	* @param 2: The VBO ID we are binding the buffer to
	*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*
	* Now we need to connect the buffer data
	* We will be binding the vertices that we just created to the GL_ARRAY_BUFFER for this VBO
	* @param 1: We want to enter which buffer we want to bind it to
	* @param 2: Size of the data
	* @param 3: Data itself
	* @param 4: Specifies the expected usage pattern of the data, there are multiple types below are two egs
	*	GL_STATIC_DRAW: The data store contents will be modified once and used many times.
	*	GL_DYNAMIC_DRAW: The data store contents will be modified repeatedly and used many times
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	* We need attribute pointer for the shader to access this data
	* @param 1: Location of the attributes we are creating this pointer for
	* @param 2: The size of each value we are going to pass in 3 in this case as we are passing in xyz 3 values
	* @param 3: The type of those values being passed in
	* @param 4: If you're using color values stored as unsigned bytes (range 0-255) but you want them to be treated as floating-point values in the shader range (unsigned integers)(0.0 - 1.0)/(signed integers)(-1.0 - 1.0), you would set normalized to GL_TRUE. If your data is already in the desired range for the shader, you would set normalized to GL_FALSE.
	* @param 5: We can have mutiple values in the same array vertices, right now we only have locations but after location we can also put colors, so we will have to pick first three floats as location then skip next three as it is color and then pick next 3, but here we have it as 0
	* @param 6: This is the offset where the data starts, we are starting immediately so 0, else if we wished to skip the first 3 value can put 3
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*
	* We have set the above attribute pointer to 0, so here we are enabling it
	*/
	glEnableVertexAttribArray(0);

	// As we did bind the buffer to our vbo now we are setting it 0 to unbind it 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// As we did bind the vertex to our vao now we are setting it 0 to unbind it
	glBindVertexArray(0);
}

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

	// We are done with setting up the properties of our GLFW window now we need to create a window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow) 
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	int bufferWidth, bufferHeight;

	/*
	* We are creating the window using width and height in screen coordinates but to set the view port size we need to have the size in pixels not in coordinate type
	* Hence we are getting the pixel size with this function which can be then passed to glviewport to set the size of viewport
	* Upper left corner of the window is considered to be point (0,0) now we have to create a window of pixel length and breadth which we can recieve from this function
	*/
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	/* Set the context for glew to use
	* We want glew to know that this is the window we have created here is the one opengl context should be tied to, so when everything gets drawn it should be drawn to this window
	* In short we are saying use the window we have just created and draw to that
	* So if we have two window we can set context to that window
	*/
	glfwMakeContextCurrent(mainWindow); 

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	// Initialise GLEW
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("GLEW Initialization failed: '%s'", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTraiangle();
	CompileShader();

	// Loop until the window is closed
	while (!glfwWindowShouldClose(mainWindow)) 
	{
		// Get + handle user input events, checks if any events happened like keyboard presses, mouse movements, moving the window and resizing; If not called we cant close or resize or do any such functionality
		glfwPollEvents();

		if (direction)triOffset += triIncrement;
		else triOffset -= triIncrement;

		if (abs(triOffset) >= triMaxOffset)direction = !direction;

		// Clear the window
		glClearColor(1.f, 1.f, 0.f, 0.75f);
		glClear(GL_COLOR_BUFFER_BIT); // Clear the color data

		// IMP: As we have multiple shaders we have to switch the shaders in and out, we have things drawn with one shader, so we draw them all and unassign the shader and draw the next shaders
		
		// Go to the graphics card and use this shader program
		glUseProgram(shaderProgram);

		glUniform1f(uniformXMove, triOffset);

		// We are binding this VAO, basically we are saying that we are working with this VAO
		glBindVertexArray(VAO);

		/*
		* We are now calling the function to draw
		* @param 1: What to draw, if we wished to draw lines we can type GL_LINES, we put triangles so now it knows it needs to fill in the space with color
		* @param 2: The first part where in array starts
		* @param 3: The count of vertices we need to draw
		*/
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Unbind the vertex array
		glBindVertexArray(0);

		// Unassign the shader
		glUseProgram(0);

		/* 
		* So we have these two scenes one which user is seeing right now and the above one in which we are drawing for the user to see next
		* Right now user has a scene in front of him, we have drawn an new scene through the above code, now we want to sawp the scene and make user see this scene
		*/
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}