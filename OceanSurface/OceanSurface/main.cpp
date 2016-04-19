#include "stdafx.h"
#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// Other includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>


#include "wave.h"
#include "Record.h"

using namespace glm;

//#include <learnopengl/filesystem.h>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void focus_callback(GLFWwindow *window, int focused);

GLFWwindowfocusfun focusFun;

void do_movement();


#define FULL_SCREEN 0
#define RECORD 0


#if RECORD	// Record

int record = 1;
#define MESH_RESOLUTION 1024

#else

int record = 0;
#define MESH_RESOLUTION 64

#endif

// Mesh resolution
int N = MESH_RESOLUTION;
int M = MESH_RESOLUTION;
float L_x = 1000;
float L_z = 1000;


float A = 3e-7f;
// Wind speed
float V = 30;
// Wind direction
vec2 omega(1, 1);

GLuint WIDTH = 1280, HEIGHT = 720;

// Camera
Camera  camera(30.0f, 30.0f, 60.0f, 0, 1, 0, 0, 0, 0);
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
bool    keys[1024];

// Light attributes
glm::vec3 lampPos(0.0f, 50, 0.0);
glm::vec3 sundir(normalize(vec3(0, 1, -2)));
glm::vec3 suncol(1.0f, 1.0f, 1.0f);
glm::vec3 seacolor(0.0f, 0.0, 0.5);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

glm::vec3* vertexArray;
GLuint surfaceVAO = 0;
GLuint surfaceVBO, EBO;
int indexSize;

float time = 0;
Wave *wave_model;

int nWaves = 60;
glm::vec4* waveParams;

float height;

float heightMax = 0;
float heightMin = 0;

void initBufferObjects()
{
	indexSize = (N - 1) * (M - 1) * 6;
	GLuint *indices = new GLuint[indexSize];

	int p = 0;

	for (int j = 0; j < N - 1; j++)
	for (int i = 0; i < M - 1; i++)
	{
		indices[p++] = i + j * N;
		indices[p++] = (i + 1) + j * N;
		indices[p++] = i + (j + 1) * N;

		indices[p++] = (i + 1) + j * N;
		indices[p++] = (i + 1) + (j + 1) * N;
		indices[p++] = i + (j + 1) * N;
	}

	// Element buffer object
	glGenVertexArrays(1, &surfaceVAO);
	glBindVertexArray(surfaceVAO);
	glGenBuffers(1, &surfaceVBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), indices, GL_STATIC_DRAW);	

	delete[] indices;
}

void deleteBufferObjects()
{
	glDeleteVertexArrays(1, &surfaceVAO);
	glDeleteBuffers(1, &surfaceVBO);
	glDeleteBuffers(1, &EBO);
}

// _WAVE_
// Build the mesh using the height provided by the algorithm.
void buildTessendorfWaveMesh(Wave* wave_model)
{	
	int nVertex = N * M;	
	
	wave_model->buildField(time);
	vec3* heightField = wave_model->heightField;
	vec3* normalField = wave_model->normalField;

	int p = 0;

	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{	
			int index = j * N + i;

			if (heightField[index].y > heightMax) heightMax = heightField[index].y;
			else if (heightField[index].y < heightMin) heightMin = heightField[index].y;
		}

	
	glBindVertexArray(surfaceVAO);	
	glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);

	int fieldArraySize = sizeof(glm::vec3) * nVertex;
	glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

	// Copy height to buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Copy normal to buffer
	glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, normalField);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)fieldArraySize);
	glEnableVertexAttribArray(1);
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	GLFWwindow* window;
#if FULL_SCREEN
	// Full screen
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(mode->width, mode->height, "My Title", glfwGetPrimaryMonitor(), NULL);

	WIDTH = mode->width;
	HEIGHT = mode->height;
#else
	//glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Ocean Surface", nullptr, nullptr);
#endif

	glfwMakeContextCurrent(window);
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowFocusCallback(window, focus_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);	


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();


	if (record)
	{
		InitiatlizeRecordBuffer();
	}
	else
	{
		glViewport(0, 0, WIDTH, HEIGHT);
	}

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

	camera.MovementSpeed = 30;

    // Build and compile our shader program
    Shader lightingShader("surface.vert", "surface.frag");
	//Shader lightingShader("lighting.vs", "lighting.glsl");
    Shader lampShader("lamp.vert", "lamp.frag");

    GLfloat vertices[] = {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

	// Light
	lampPos = sundir * 50.0f;
    GLuint VBO;
	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);		
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	initBufferObjects();

	// _WAVE_
	// Speed of wind, direction of wind
	time = 0;
	float modelScale = 0.1;
	wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);


	// Use this function to draw the wire frame.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
		//printf("fps: %f\n", 1 / deltaTime);
        lastFrame = currentFrame;

		// _WAVE_
		time += 0.20;
		buildTessendorfWaveMesh(wave_model);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();
		
		GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);	
		glUniform1f(glGetUniformLocation(lightingShader.Program, "heightMin"), heightMin * modelScale);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "heightMax"), heightMax * modelScale);		

		// Set lights properties
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 0.9f, 0.7f);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);		

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc  = glGetUniformLocation(lightingShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));		

		// ===== Draw Model =====
		glBindVertexArray(surfaceVAO);
        glm::mat4 model;
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(modelScale));	// Scale the surface
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));       
		
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


		// ===== Draw Lamp =====

        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use();
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc  = glGetUniformLocation(lampShader.Program, "view");
        projLoc  = glGetUniformLocation(lampShader.Program, "projection");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4();
        model = glm::translate(model, lampPos);
        model = glm::scale(model, glm::vec3(1.0f)); // Make it a smaller cube
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

		// Record
		if (record)
		{
			glfwSetCursorPosCallback(window, NULL);
			glfwSetScrollCallback(window, NULL);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			if (RecordFrame() == RECORD_FINISHED) break;
		}
		else
		{
			// Swap the screen buffers
			glfwSwapBuffers(window);
		}

		
    }

	CleanUpRecord();
	deleteBufferObjects();
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_Z)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (key == GLFW_KEY_UP)
	{
		A *= 2;
		wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
	}
	if (key == GLFW_KEY_DOWN)
	{
		A /= 2;
		wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
	}
	if (key == GLFW_KEY_X)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_LEFT_SHIFT])
		camera.ProcessKeyboard(SHIFT, deltaTime);
	if (keys[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(SPACE, deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//printf("%f %f\n", xpos, ypos);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void focus_callback(GLFWwindow *window, int focused)
{
	printf("Focus: %d\n", focused);
	if (focused == GL_FALSE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		printf("Cursor set to normal. \n", focused);
	}

}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		printf("Mouse_Button. \nCursor set to disabled.\n", button);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}
