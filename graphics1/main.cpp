// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// .obj loading dependency
#include "objloader.cpp"

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
glm::vec3 position = glm::vec3(50, 50, 500);
glm::vec3 Look = glm::vec3(50, 50, 50);
// Initial horizontal angle
float horizontalAngle = 3.14f;
// Initial vertical angle
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;
// zoom for FoV change
float zoomAngle = 1.0f;
// Positition of camera head in world space
float head_pos = 1.0f;
// Angle of camera head
float head_ang = 0.1f;
// Movement speed; arbitrary value of 150 u/s
float speed = 150.0f;

void computeMatricesFromInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Direction vector (Doesn't work, head doesnt cooperate)
	/* glm::vec3 direction(
		//cos(verticalAngle) * sin(horizontalAngle),
		0,
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		//2 * cos(verticalAngle - 3.14f / 2.0f)
	);*/

	// Right vector
	glm::vec3 right = glm::vec3(
		2 * sin(horizontalAngle - 3.14f / 2.0f),
		0,
		2 * cos(horizontalAngle - 3.14f / 2.0f)
	);
	/*
	// Rotate up
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		verticalAngle += 0.01;
		head_ang += 0.01;
		position += direction * deltaTime * speed;
	}
	// Rotate down
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		verticalAngle -= 0.01;
		head_ang += 0.01;
		position -= direction * deltaTime * speed;
	}
	*/
	// Rotate right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalAngle += (float)0.01;
		position += right * deltaTime * speed;
	}
	// Rotate left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalAngle -= (float)0.01;
		position -= right * deltaTime * speed;
	}
	// Zoom in
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		initialFoV -= zoomAngle * deltaTime * speed * 0.1f;
	}
	// zoom out
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		initialFoV += zoomAngle * deltaTime * speed * 0.1f;
	}
	if (head_ang - (0.785 * floor(head_ang / 0.785)) == 0) {
		head_pos = head_pos * -1;
	}


	float FoV = initialFoV;

	// Projection matrix : gets dynamic FoV for zoom
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 1.0f / 1.0f, 0.1f, 1000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // camera is here
		Look,			    // looks here
		vec3(0, head_pos, 0)// Head is up or down
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



	GLuint vao[2];
	glGenVertexArrays(2, vao);


	GLuint programID = LoadShaders("core.vs", "core.fs");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 1:1 ratio, display range : 0.1 unit <-> 1000 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 1000.0f);

	// Camera matrix
	glm::mat4 View = ViewMatrix;
	/* glm::mat4 View = glm::lookAt(
		glm::vec3(-15, 0, -310), // Camera is at (-15, 0, -310) in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up
	);
	View = glm::rotate(View, glm::radians(40.0f), glm::vec3(1.0, 0.0, 0.0));
	*/

	// read the scene cube
	std::vector<glm::vec3> scnCubeVert;
	std::vector<glm::vec2> scnCubeUvs;
	std::vector<glm::vec3> scnCubeNormals;
	bool res1 = loadOBJ("scnCube.obj", scnCubeVert, scnCubeUvs, scnCubeNormals);

	// read the big sphere
	std::vector<glm::vec3> scnSphVert;
	std::vector<glm::vec2> scnSphUvs;
	std::vector<glm::vec3> scnSphNormals;
	bool res2 = loadOBJ("SPH.obj", scnSphVert, scnSphUvs, scnSphNormals);

	// read the cylinder
	std::vector<glm::vec3> cylVert;
	std::vector<glm::vec2> cylUvs;
	std::vector<glm::vec3> cylNormals;
	bool res3 = loadOBJ("cylinder.obj", cylVert, cylUvs, cylNormals);

	// read the sphere
	std::vector<glm::vec3> sphVert;
	std::vector<glm::vec2> sphUvs;
	std::vector<glm::vec3> sphNormals;
	bool res4 = loadOBJ("sphere.obj", sphVert, sphUvs, sphNormals);

	// read the cube
	std::vector<glm::vec3> cubVert;
	std::vector<glm::vec2> cubUvs;
	std::vector<glm::vec3> cubNormals;
	bool res5 = loadOBJ("cube.obj", cubVert, cubUvs, cubNormals);

	// read the sphere texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("texture-sphere.jpg", &width, &height, &nrChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);


	glBindTexture(GL_TEXTURE_2D, texture);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load sphere texture" << std::endl;
	}
	stbi_image_free(data);
	
	
	


	// Our Model matrix
	glm::mat4 Model = glm::mat4(1.0f);


	// Our ModelViewProjection matrix
	glm::mat4 MVP = Projection * View * Model;



	// random colour for the scene cube
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);

	float red = dis(gen);
	float gr = dis(gen);
	float bl = dis(gen);
	float trans = 0.5f; // this is the transparency of the scn cube

	//////////////////
	// vert sizes:
	// cube 36
	// sphere 2880
	// cylinder 372
	//////////////////

	//GLfloat vbo[2];

	GLfloat scnCubeColorBufferData[36 * 4];

	for (int i = 0; i < 36; i++) {
		int j = i * 4;
		scnCubeColorBufferData[j] = red;
		scnCubeColorBufferData[j + 1] = gr;
		scnCubeColorBufferData[j + 2] = bl;
		scnCubeColorBufferData[j + 3] = trans;
	}


	GLfloat scnSphColorBufferData[2880 * 4];

	for (int i = 0; i < 2880; i++) {
		int j = i * 4;
		scnSphColorBufferData[j] = 1;
		scnSphColorBufferData[j + 1] = 0;
		scnSphColorBufferData[j + 2] = 0;
		scnSphColorBufferData[j + 3] = 1;
	}


	// these are for the scene cube

	GLuint scnCubeVertexbuffer;
	glGenBuffers(1, &scnCubeVertexbuffer);

	GLuint scnCubeColorbuffer;
	glGenBuffers(1, &scnCubeColorbuffer);

	glBindVertexArray(vao[0]);

	// scn cube vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, scnCubeVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, scnCubeVert.size() * sizeof(glm::vec3), &scnCubeVert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	// scn cube colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, scnCubeColorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scnCubeColorBufferData), scnCubeColorBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);

	// these are for the big red sphere

	GLuint scnSphVertexbuffer;
	glGenBuffers(1, &scnSphVertexbuffer);

	GLuint scnSphColorbuffer;
	glGenBuffers(1, &scnSphColorbuffer);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);

	glBindVertexArray(vao[1]);

	// SPH vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, scnSphVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, scnSphVert.size() * sizeof(glm::vec3), &scnSphVert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	// SPH colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, scnSphColorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scnSphColorBufferData), scnSphColorBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	// SPH uv buffer (texture)
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, scnSphUvs.size() * sizeof(glm::vec2), &scnSphUvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);


	// Enable blending used in transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool spawn = true;

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

		// camera code before this line

		// draw the scn cube
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 360);


		if (glfwGetKey(window, GLFW_KEY_SPACE) == true)
		{
			spawn = !spawn;
		}

		if (spawn == true)
		{
			// draw the big red sphere
			glBindVertexArray(vao[1]);
			glDrawArrays(GL_TRIANGLES, 0, 2880);
		}




		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // ESC key/window closed check
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &scnCubeVertexbuffer);
	glDeleteBuffers(1, &scnCubeColorbuffer);
	glDeleteBuffers(1, &scnSphVertexbuffer);
	glDeleteBuffers(1, &scnSphColorbuffer);
	glDeleteVertexArrays(2, vao);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}