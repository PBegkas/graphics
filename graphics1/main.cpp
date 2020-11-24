// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

//hello from iolkos
//hello from NDG

// random lib is used to create random colours for the scene cube
#include <random>

// Include GLEW/GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

// shader files
#include "shaders.hpp"

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::mat4 ModelMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 10);
glm::vec3 zoom = glm::vec3(0, 0, 0);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;

// Initial Field of View
float initialFoV = 45.0f;
//float roundAngle = 0.0f;
float upAng = 1.0f;

float speed = 3.0f; // 3 units / second

void computeMatricesFromInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	//roundAngle += deltaTime;

	// Direction vector
	glm::vec3 direction(
		//cos(verticalAngle) * sin(horizontalAngle),
		0,
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		//2 * cos(verticalAngle - 3.14f / 2.0f)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		2* sin(horizontalAngle - 3.14f / 2.0f),
		0,
		2* cos(horizontalAngle - 3.14f / 2.0f)
	);
	

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		verticalAngle += 0.01;
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		verticalAngle -= 0.01;
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalAngle += (float)0.01;
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalAngle -= (float)0.01;
		position -= right * deltaTime * speed;
	}
	// Zoom in
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		initialFoV -= upAng * deltaTime * speed * 2.0f;
	}
	// zoom out
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		initialFoV += upAng * deltaTime * speed * 2.0f;
	}



	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 1.0f / 1.0f, 0.1f, 1000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		//glm::vec3(0, 0, 0) , // and looks here : at the same position, plus "direction"
		//position + direction,
		zoom,
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //  MacOS compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window making process
	window = glfwCreateWindow(600, 600, "Συγκρουόμενα", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Escape key
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("core.vs", "core.fs");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 1:1 ratio, display range : 0.1 unit <-> 500 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 500.0f);

	// Camera matrix
	glm::mat4 View = ViewMatrix;
	/* glm::mat4 View = glm::lookAt(
		glm::vec3(-15, 0, -310), // Camera is at (-15, 0, -310) in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up 
	);

	View = glm::rotate(View, glm::radians(40.0f), glm::vec3(1.0, 0.0, 0.0));
	*/
	// Our Model matrix
	glm::mat4 Model = glm::mat4(1.0f);

	// scale the model x50
	Model = glm::scale(Model, glm::vec3(50.0f, 50.0f, 50.0f));

	// "move" the cube
	Model = glm::translate(Model, glm::vec3(1.0f, 1.0f, 1.0f));

	// Our ModelViewProjection matrix
	glm::mat4 MVP = Projection * View */* transformation * translateMatrix  *  scaleMatrix */  Model;

	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f,-1.0f,-1.0f, 
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f,-1.0f, 
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, 
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
	};

	// random colour for the scene cube
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);

	float red = dis(gen);
	float gr = dis(gen);
	float bl = dis(gen);
	

	static const GLfloat g_color_buffer_data[]{
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
		red, gr, bl,
	};


	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Enable blending used in transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	do {
		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation, to the currently bound shader,  in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // not normalized
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute 1, matches the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // not normalized
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Triangle drawing
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // ESC key/window closed check
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}