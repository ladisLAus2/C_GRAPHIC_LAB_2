#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Sphere.h"
#include "Camera.h"
#include <iostream>
#include <glut.h>
#include <Windows.h>
#include "Graph.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
bool isPerspective = true;
float deltaTime = 0.0f;	
float lastFrame = 0.0f;
const float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
const float n = glm::radians(90.0f);

Sphere sphere(1.0f, 36, 18);

Graph* plot = new Graph();

float PlaneVertices[] = {
	 10.5f,  10.5f, 1.5f,
	 10.5f, -10.5f, 1.5f,
	-10.5f, -10.5f, 1.5f,
	-10.5f,  10.5f, 1.5f
};

unsigned int Planeindices[] = {
0, 1, 3,
1, 2, 3
};

float verticesOktaeder[] = {
  1.0f,  0.0f,  0.0f, 0.5f, 0.0f,
 -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
  0.0f,  0.0f,  1.0f, 0.0f, 0.5f
};

unsigned int indicesOktaeder[] = {
0, 2, 5,
2, 1, 5,
1, 3, 5,
0, 3, 5,

0, 4, 2,
2, 4, 1,
1, 4, 3,
3, 4, 0
};


int main()
{	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OPENGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "error creating the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "error initialization GLAD" << std::endl;
	}


	glEnable(GL_DEPTH_TEST);

	Shader PrimitivesShader("FiguresVertexShader.vs", "FiguresFragmentShader.fs");
	Shader ColorShader("ColorVertexShader.vs", "ColorFragmentShader.fs");

	unsigned int VBO[4], VAO[4], EBO[4];
	glGenVertexArrays(4, VAO);
	glGenBuffers(4, VBO);
	glGenBuffers(4, EBO);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sphere.getInterleavedVertexSize(), sphere.getInterleavedVertices(), GL_STATIC_DRAW);//all vertices

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW);//indices

	int stride = sphere.getInterleavedStride();

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0);//position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));//texture

	

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PlaneVertices), PlaneVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Planeindices), Planeindices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);


	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesOktaeder), verticesOktaeder, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesOktaeder), indicesOktaeder, GL_STATIC_DRAW);


	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5*sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, plot->vertices.size()* sizeof(float), plot->vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)plot->indices.size() * sizeof(unsigned int), plot->indices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load("8k_jupiter.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("2k_venus_surface.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	PrimitivesShader.use();
	PrimitivesShader.setInt("texture", 0);

	while (!glfwWindowShouldClose(window))
	{
		PrimitivesShader.setFloat("aColor", glm::vec3(0.0f, 0.0f, 0.0f));
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window);
		glClearColor(0.2f, 0.2f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 projection;

		if (isPerspective)//is orthogonal or perspective view
			projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		else
			projection = glm::ortho(-n, n, -n * aspect, n * aspect, 0.1f, 100.0f);

		glBindVertexArray(VAO[0]);
		PrimitivesShader.use();
		glm::mat4 modelSphere = glm::mat4(1.0f);
		modelSphere = glm::translate(modelSphere, glm::vec3(-3.0f, 0.f, 0.f));
		modelSphere = glm::rotate(modelSphere, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		float angle = 23.0f * glfwGetTime();
		modelSphere = glm::rotate(modelSphere, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		PrimitivesShader.setMat4("projection", projection);
		PrimitivesShader.setMat4("view", view);
		PrimitivesShader.setMat4("model", modelSphere);
		glDrawElements(GL_TRIANGLES, sphere.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

		
		glBindVertexArray(VAO[1]);
		ColorShader.use();
		glm::mat4 modelPlane = glm::mat4(1.0f);
		modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, -3.f, 0.f));
		modelPlane = glm::rotate(modelPlane, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ColorShader.setFloat("aColor", glm::vec3(0.6f, 0.6f, 0.6f));
		ColorShader.setMat4("view", view);
		ColorShader.setMat4("projection", projection);
		ColorShader.setMat4("model", modelPlane);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glBindVertexArray(VAO[2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		PrimitivesShader.use();
		glm::mat4 OktaedrModel = glm::mat4(1.0f);
		angle = 45 * glfwGetTime();
		OktaedrModel = glm::translate(OktaedrModel, glm::vec3(3.0f, 0.f, 0.f));
		OktaedrModel = glm::rotate(OktaedrModel, glm::radians(angle), glm::vec3(1.0f, 0.0f, .0f));
		PrimitivesShader.setMat4("model", OktaedrModel);
		glDrawElements(GL_TRIANGLES, sizeof(indicesOktaeder), GL_UNSIGNED_INT, 0);


		glBindVertexArray(VAO[3]);
		ColorShader.use();
		glm::mat4 modelPlot = glm::mat4(1.0f);
		modelPlot = glm::translate(modelPlot, glm::vec3(0.0f, 0.f, -3.f));
		modelPlot = glm::rotate(modelPlot, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ColorShader.setMat4("model", modelPlot);
		ColorShader.setFloat("aColor", glm::vec3(0.0f, 0.5f, 0.3f));
		glDrawElements(GL_TRIANGLE_STRIP, (unsigned int)plot->indices.size(), GL_UNSIGNED_INT, 0);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(4, VAO);
	glDeleteBuffers(4, VBO);
	glDeleteBuffers(4, EBO);
	
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		isPerspective = true;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		isPerspective = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

