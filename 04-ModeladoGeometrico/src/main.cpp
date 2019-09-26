//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes

#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"


//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

GLFWwindow * window;

Shader shader;
std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20), sphere2(20, 20), sphere3(20, 20), sphere4(20, 20), sphere5(20, 20), sphere6(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5), cylinder2(20, 20, 0.5, 0.5), cylinder3(20, 20, 0.5, 0.5);
Box box1;


bool exitApp = false;
int lastMousePosX, offsetX=0;
int lastMousePosY, offsetY=0;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {
	
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	//inicializar los buffers VAO, VBO. EBO
	sphere1.init();
	//metodo setter que coloca el apuntador al shader que vamos a ocupar
	sphere1.setShader(&shader);
	//setter para poner el color de la geometria 
	sphere1.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	//inicializar los buffers VAO, VBO. EBO
	sphere2.init();
	//metodo setter que coloca el apuntador al shader que vamos a ocupar
	sphere2.setShader(&shader);
	//setter para poner el color de la geometria 
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	//pupilas
	sphere3.init();
	sphere3.setShader(&shader);
	sphere3.setColor(glm::vec4(0.0, 0.0, 0.0, 1.0));

	sphere4.init();
	sphere4.setShader(&shader);
	sphere4.setColor(glm::vec4(0.0, 0.0, 0.0, 1.0));

	//iris
	sphere5.init();
	sphere5.setShader(&shader);
	sphere5.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	sphere6.init();
	sphere6.setShader(&shader);
	sphere6.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shader);
	cylinder1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	//pantalon
	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec4(0.75, 0.54, 0.33, 1.0));

	//camisa
	cylinder3.init();
	cylinder3.setShader(&shader);
	cylinder3.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	box1.init();
	box1.setShader(&shader);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
	camera->setPosition(glm::vec3(2.0, 0.0, 4.0));
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	//destruccion de los VAO, EBO. VBO
	sphere1.destroy();
	cylinder1.destroy();
	box1.destroy();

	shader.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication){
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime); //deltaTime es cuanto ha pasado despues de que refresque mi pantalla
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, 0.01);
	offsetX = 0;
	offsetY = 0;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();	//METODO QUE OBTIENE LA MATRIZ DE VISTA

		shader.turnOn();

		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);

		//visualizar con lineas
		//sphere1.enableWireMode();
		//Dibujado de la geometria, recibe la matriz de transformacion 
		//sphere1.render(model);
		

		/*cylinder1.render(model);
		cylinder1.enableWireMode();*/
		


		//box1.enableWireMode();
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));

		//pantalon
		glm::mat4 p1 = glm::translate(model, glm::vec3(0.0, -0.375, 0.05f));
		//cylinder2.enableWireMode();
		cylinder2.render(glm::scale(p1, glm::vec3(1.0, 0.25, 0.01)));

		//camisa
		glm::mat4 c1 = glm::translate(model, glm::vec3(0.0, -0.25, 0.05f));
		//cylinder2.enableWireMode();
		cylinder3.render(glm::scale(c1, glm::vec3(1.0, 0.25, 0.01)));

		//articulacion 
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		sphere1.enableWireMode();	 
		sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));

		//hueso1
		glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0, 0.0));
		l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l1, glm::vec3(0.1, 0.5, 0.1)));

		//articulacion 2 (codo)
		//depende de j1 porque queremos que si se mueve j1 se mueva esta esfera
		glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

		//hueso2 
		glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25f, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.5, 0.1)));
		shader.turnOff();

		//articulacion 3
		glm::mat4 j3 = glm::translate(model, glm::vec3(-0.5, 0.f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));

		//hueso3 
		glm::mat4 l3 = glm::translate(j3, glm::vec3(-0.25f, 0.0, 0.0));
		l3 = glm::rotate(l3, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));
		shader.turnOff();

		//articulacion 4
		//depende de j3 porque queremos que si se mueve j3 se mueva esta esfera
		glm::mat4 j4 = glm::translate(j3, glm::vec3(-0.5, 0.f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));

		//hueso4 
		glm::mat4 l4 = glm::translate(j4, glm::vec3(-0.25f, 0.0, 0.0));
		l4 = glm::rotate(l4, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l4, glm::vec3(0.1, 0.5, 0.1)));
		shader.turnOff();

		//articulacion 5 (pierna izq)
		glm::mat4 j5 = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));

		//hueso 5 
		glm::mat4 l5 = glm::translate(j5, glm::vec3(0.0f, -0.25, 0.0));
		l5 = glm::rotate(l5, glm::radians(180.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l5, glm::vec3(0.1, 0.5, 0.1)));

		//articulacion 6 (pierna der)
		glm::mat4 j6 = glm::translate(model, glm::vec3(0.25f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j6, glm::vec3(0.1, 0.1, 0.1)));

		//hueso 6
		glm::mat4 l6 = glm::translate(j6, glm::vec3(0.0f, -0.25, 0.0));
		l6 = glm::rotate(l6, glm::radians(180.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l6, glm::vec3(0.1, 0.5, 0.1)));

		//ojo
		glm::mat4 ojo = glm::translate(model, glm::vec3(0.25f, 0.25, 0.05));
		//sphere1.enableWireMode();
		sphere1.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));

		//ojo2
		glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25f, 0.25, 0.05));
		//sphere2.enableWireMode();
		sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));

		//ojo (pupila)
		glm::mat4 pupila = glm::translate(ojo, glm::vec3(0.0f, 0.0, 0.055));
		//sphere1.enableWireMode();
		sphere3.render(glm::scale(pupila, glm::vec3(0.05, 0.05, 0.01)));

		//ojo2 (pupila)
		glm::mat4 pupila2 = glm::translate(ojo2, glm::vec3(0.0f, 0.0, 0.055));
		//sphere2.enableWireMode();
		sphere4.render(glm::scale(pupila2, glm::vec3(0.05, 0.05, 0.01)));

		//ojo (iris)
		glm::mat4 iris = glm::translate(ojo, glm::vec3(0.0, 0.0, 0.05));
		//sphere1.enableWireMode();
		sphere5.render(glm::scale(iris, glm::vec3(0.1, 0.1, 0.01)));

		//ojo2 (iris)
		glm::mat4 iris2 = glm::translate(ojo2, glm::vec3(0.0f, 0.0, 0.05));
		//sphere2.enableWireMode();
		sphere6.render(glm::scale(iris2, glm::vec3(0.1, 0.1, 0.01)));

		//nariz
		glm::mat4 nariz = glm::translate(model, glm::vec3(0.0f, 0.15, 0.2));
		nariz = glm::rotate(nariz, glm::radians(90.0f), glm::vec3(1.0, 0.0f, 0.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(nariz, glm::vec3(0.1, 0.375, 0.1)));

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
