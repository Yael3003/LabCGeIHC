#define _USE_MATH_DEFINES
#include <cmath>
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

//Descomentar
#include "Headers/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader de texturizado
Shader shaderTexture;
//Shader con iluminacion solo color
Shader shaderColorLighting;
//Shader con iluminacion y textura
Shader shaderTextureLighting;
// Descomentar
//Shader con materiales
Shader shaderMaterialLighting;
// Descomentar
//Shader con skybox
Shader shaderSkybox;
// Shader con multiplesluces
Shader shaderMulLighting;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Sphere sphereLamp(20, 20);
// Descomentar
Sphere skyboxSphere(20, 20);
Cylinder pata(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
// Descomentar
Cylinder cylinderMaterials(20, 20, 0.5, 0.5);
Box box1;
Box box2;
Box box3;
Box boxMaterials;
Box pared1, piso1, techo1;
//Para los sofas
Box sofabk, sofac1, sofac2, sofac3, sofac4, sofac5, sofal, sofar;
//Para mesa y muebles 
Box mesa, mueble1Sup, mueble1Inf;

GLuint textureID1, textureID2, textureID3, textureID4;
// Descomentar
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_perdicus/perdicus_ft.tga",
		"../Textures/mp_perdicus/perdicus_bk.tga",
		"../Textures/mp_perdicus/perdicus_up.tga",
		"../Textures/mp_perdicus/perdicus_dn.tga",
		"../Textures/mp_perdicus/perdicus_rt.tga",
		"../Textures/mp_perdicus/perdicus_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float rot0 = 0.0, dz = 0.0;

float rot1 = 0.0, rot2 = 0.0, rot3 = 0.0, rot4 = 0.0;
bool sentido = true;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
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
	shaderTexture.initialize("../Shaders/texturizado_res.vs",
			"../Shaders/texturizado_res.fs");
	shaderColorLighting.initialize("../Shaders/iluminacion_color_res.vs",
			"../Shaders/iluminacion_color_res.fs");
	shaderTextureLighting.initialize("../Shaders/iluminacion_texture_res.vs",
			"../Shaders/iluminacion_texture_res.fs");
	// Descomentar
	shaderMaterialLighting.initialize("../Shaders/iluminacion_material.vs",
	 "../Shaders/iluminacion_material.fs");
	// Descomentar
	shaderSkybox.initialize("../Shaders/cubeTexture.vs",
			"../Shaders/cubeTexture.fs");
	shaderMulLighting.initialize("../Shaders/Iluminacion_texture_res.vs",
		"../Shaders/multipleLights.fs");


	// Inicializar los buffers VAO, VBO, EBO
	sphere1.init();
	// Método setter que colocar el apuntador al shader
	sphere1.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphereLamp.init();
	// Método setter que colocar el apuntador al shader
	sphereLamp.setShader(&shader);
	//Setter para poner el color de la geometria
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	pata.init();
	pata.setShader(&shader);
	pata.setColor(glm::vec4(0.12, 0.1, 0.15, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderTextureLighting);

	// Descomentar
	cylinderMaterials.init();
	 cylinderMaterials.setShader(&shaderMaterialLighting);
	 cylinderMaterials.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	 boxMaterials.init();
	 boxMaterials.setShader(&shaderMaterialLighting);


	// Descomentar
	//inicializacion de la esfera del skybox
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	box1.init();
	// Settea el shader a utilizar
	box1.setShader(&shaderTextureLighting);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	box2.init();
	box2.setShader(&shaderTextureLighting);

	sphere3.init();
	sphere3.setShader(&shaderTextureLighting);

	box3.init();
	box3.setShader(&shaderTextureLighting);

	pared1.init();
	// Settea el shader a utilizar
	pared1.setShader(&shader);
	pared1.setColor(glm::vec4(0.85, 0.8, 0.4, 1.0));

	piso1.init();
	// Settea el shader a utilizar
	piso1.setShader(&shaderTextureLighting);
	piso1.setColor(glm::vec4(0.3, 0.2, 0.25, 1.0));

	//sofa1
	sofabk.init();
	// Settea el shader a utilizar
	sofabk.setShader(&shader);
	sofabk.setColor(glm::vec4(0.5, 0.4, 0.2, 1.0));
	
	sofac1.init();
	// Settea el shader a utilizar
	sofac1.setShader(&shader);
	sofac1.setColor(glm::vec4(0.6, 0.5, 0.3, 1.0));
	
	sofac2.init();
	// Settea el shader a utilizar
	sofac2.setShader(&shader);
	sofac2.setColor(glm::vec4(0.65, 0.55, 0.35, 1.0));

	sofac3.init();
	// Settea el shader a utilizar
	sofac3.setShader(&shader);
	sofac3.setColor(glm::vec4(0.6, 0.5, 0.3, 1.0));

	sofac4.init();
	// Settea el shader a utilizar
	sofac4.setShader(&shader);
	sofac4.setColor(glm::vec4(0.65, 0.55, 0.35, 1.0));

	sofac5.init();
	// Settea el shader a utilizar
	sofac5.setShader(&shader);
	sofac5.setColor(glm::vec4(0.6, 0.5, 0.3, 1.0));
	
	sofar.init();
	// Settea el shader a utilizar
	sofar.setShader(&shader);
	sofar.setColor(glm::vec4(0.5, 0.4, 0.2, 1.0));
	sofal.init();
	// Settea el shader a utilizar
	sofal.setShader(&shader);
	sofal.setColor(glm::vec4(0.5, 0.4, 0.2, 1.0));

	mesa.init();
	// Settea el shader a utilizar
	mesa.setShader(&shader);
	mesa.setColor(glm::vec4(0.5, 0.4, 0.2, 1.0));

	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));

	// Descomentar
	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	// Definiendo la textura a utilizar
	Texture texture1("../Textures/losetas.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	FIBITMAP *bitmap = texture1.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	unsigned char *data = texture1.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID1);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture1.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture2("../Textures/water.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture2.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture2.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID2);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture2.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture3("../Textures/goku.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture3.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture3.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID3);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture3.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture4("../Textures/texturaLadrillos.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture4.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID4);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture4.freeImage(bitmap);

	// Descomentar
	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE_MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Destrucción de los VAO, EBO, VBO
	sphere1.destroy();
	pata.destroy();
	box1.destroy();

	shader.destroy();
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
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

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
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

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sentido = false;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
		rot1 += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !sentido)
		rot1 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rot2 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		rot2 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && sentido)
		rot3 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && sentido)
		rot4 += 0.001;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot0 = 0.0001;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot0 = -0.0001;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		dz = 0.0001;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		dz = -0.0001;

	sentido = true;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	//glm::mat4 model = glm::mat4(1.0f);
	float offX = 0.0;
	float angle = 0.0;
	float ratio = 5.0;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con solo textura
		shaderTexture.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion solo color
		shaderColorLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderColorLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con textura
		shaderTextureLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTextureLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con material
		// Descomentar
		shaderMaterialLighting.setMatrix4("projection", 1, false,
		 glm::value_ptr(projection));
		 shaderMaterialLighting.setMatrix4("view", 1, false,
		 glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		// Descomentar
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		//Settea la matriz de vista y proyeccion al shader con mult.luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));

		// Propiedades de la luz para objetos con color
		shaderColorLighting.setVectorFloat3("viewPos",
				glm::value_ptr(camera->getPosition()));
		shaderColorLighting.setVectorFloat3("light.ambient",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.diffuse",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.specular",
				glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("viewPos",
				glm::value_ptr(camera->getPosition()));
		shaderTextureLighting.setVectorFloat3("light.ambient",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.diffuse",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.specular",
				glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		// Descomentar
		shaderMaterialLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		 shaderMaterialLighting.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 shaderMaterialLighting.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		 shaderMaterialLighting.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));

		 //Propiedades de la luz para los objetos con multiples luces
		 shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		 shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		 shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		 shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(0.0, 0.0, -1.0)));

		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle,
				glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix,
				glm::vec3(0.0f, 0.0f, -ratio));
		// Posicion luz para objetos con color
		shaderColorLighting.setVectorFloat3("light.position",
				glm::value_ptr(
						glm::vec4(
								lightModelmatrix
										* glm::vec4(0.0, 0.0, 0.0, 1.0))));

		// Posicion luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("light.position",
				glm::value_ptr(
						glm::vec4(
								lightModelmatrix
										* glm::vec4(0.0, 0.0, 0.0, 1.0))));


		glm::mat4 paredT = glm::mat4(1.0f);
		paredT = glm::translate(paredT, glm::vec3(0, 0, 0));
		paredT = glm::rotate(paredT, rot0, glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredT, glm::vec3(25.0, 7.5, 0.2)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredCen = glm::mat4(1.0f);
		paredCen = glm::translate(paredT, glm::vec3(0, 0, 9));
		paredCen = glm::rotate(paredCen, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredCen, glm::vec3(6.0, 7.5, 0.3)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredIzq = glm::mat4(1.0f);
		paredIzq = glm::translate(paredT, glm::vec3(-10, 0, 6));
		paredIzq = glm::rotate(paredIzq, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredIzq, glm::vec3(12.0, 7.5, 0.3)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredDer = glm::mat4(1.0f);
		paredDer = glm::translate(paredT, glm::vec3(10, 0, 6));
		paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredDer, glm::vec3(12.0, 7.5, 0.3)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 piso = glm::translate(paredT, glm::vec3(0, -3.75, 6));
		piso = glm::rotate(piso, glm::radians(90.0f), glm::vec3(1.0, 0, 0.0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		piso1.render(glm::scale(piso, glm::vec3(20.0, 12.0, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaTras = glm::translate(paredT, glm::vec3(-5, -2.5, 0.4));
		sofaTras = glm::rotate(sofaTras, rot0, glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofaTras, glm::vec3(4.0, 2.5, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaIzq = glm::translate(sofaTras, glm::vec3(-2.0, -0.5, 1.0));
		sofaIzq = glm::rotate(sofaIzq, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofaIzq, glm::vec3(2.0, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaDer = glm::translate(sofaTras, glm::vec3(2.0, -0.5, 1.0));
		sofaDer = glm::rotate(sofaDer, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofaDer, glm::vec3(2.0, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin1 = glm::translate(sofaTras, glm::vec3(0.0, -0.8, 1.2));
		sofaCojin1 = glm::rotate(sofaCojin1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofaCojin1, glm::vec3(3.8, 2.0, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin2 = glm::translate(sofaTras, glm::vec3(0.0, -0.4, 1.2));
		sofaCojin2 = glm::rotate(sofaCojin2, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofaCojin2, glm::vec3(3.8, 2.0, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin3 = glm::translate(sofaTras, glm::vec3(-1.23, 0.55, 0.2));
		sofaCojin3 = glm::rotate(sofaCojin3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac3.render(glm::scale(sofaCojin3, glm::vec3(1.3, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin4 = glm::translate(sofaTras, glm::vec3(0, 0.55, 0.2));
		sofaCojin4 = glm::rotate(sofaCojin4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac4.render(glm::scale(sofaCojin4, glm::vec3(1.3, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin5 = glm::translate(sofaTras, glm::vec3(1.23, 0.55, 0.2));
		sofaCojin5 = glm::rotate(sofaCojin5, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac5.render(glm::scale(sofaCojin5, glm::vec3(1.3, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//Segundo sofa--------------------------------------------------------

		glm::mat4 sofaTras2 = glm::translate(paredT, glm::vec3(-5, -2.5, 11));
		sofaTras2 = glm::rotate(sofaTras2, glm::radians(180.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofaTras2, glm::vec3(3.0, 2.5, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaIzq2 = glm::translate(sofaTras2, glm::vec3(-1.5, -0.5, 1.0));
		sofaIzq2 = glm::rotate(sofaIzq2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofaIzq2, glm::vec3(2.0, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaDer2 = glm::translate(sofaTras2, glm::vec3(1.5, -0.5, 1.0));
		sofaDer2 = glm::rotate(sofaDer2, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofaDer2, glm::vec3(2.0, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin1 = glm::translate(sofaTras2, glm::vec3(0.0, -0.8, 1.2));
		sofa2Cojin1 = glm::rotate(sofa2Cojin1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofa2Cojin1, glm::vec3(2.8, 2.0, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin2 = glm::translate(sofaTras2, glm::vec3(0.0, -0.4, 1.2));
		sofa2Cojin2 = glm::rotate(sofa2Cojin2, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofa2Cojin2, glm::vec3(2.8, 2.0, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin3 = glm::translate(sofaTras2, glm::vec3(-0.7, 0.55, 0.2));
		sofa2Cojin3 = glm::rotate(sofa2Cojin3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac3.render(glm::scale(sofa2Cojin3, glm::vec3(1.3, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin4 = glm::translate(sofaTras2, glm::vec3(0.7, 0.55, 0.2));
		sofa2Cojin4 = glm::rotate(sofa2Cojin4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac4.render(glm::scale(sofa2Cojin4, glm::vec3(1.3, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//----------------Tercer sofa-------------------------------------------

		glm::mat4 sofaTras3 = glm::translate(paredT, glm::vec3(-1.2, -2.5, 6));
		sofaTras3 = glm::rotate(sofaTras3, glm::radians(270.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofaTras3, glm::vec3(1.5, 2.5, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaIzq3 = glm::translate(sofaTras3, glm::vec3(-0.75, -0.5, 1.0));
		sofaIzq3 = glm::rotate(sofaIzq3, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofaIzq3, glm::vec3(1.8, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaDer3 = glm::translate(sofaTras3, glm::vec3(.75, -0.5, 1.0));
		sofaDer3 = glm::rotate(sofaDer3, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofaDer3, glm::vec3(1.8, 1.5, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin1 = glm::translate(sofaTras3, glm::vec3(0.0, -0.8, 1.2));
		sofa3Cojin1 = glm::rotate(sofa3Cojin1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofa3Cojin1, glm::vec3(1.3, 1.6, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin2 = glm::translate(sofaTras3, glm::vec3(0.0, -0.4, 1.2));
		sofa2Cojin3 = glm::rotate(sofa3Cojin2, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofa2Cojin3, glm::vec3(1.3, 1.6, 0.4)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin3 = glm::translate(sofaTras3, glm::vec3(-0.0, 0.55, 0.2));
		sofa3Cojin3 = glm::rotate(sofa3Cojin3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac3.render(glm::scale(sofa3Cojin3, glm::vec3(1.1, 1.4, 0.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 mesa1 = glm::translate(paredT, glm::vec3(5, -2.5, 6));
		mesa1 = glm::rotate(mesa1, glm::radians(270.0f), glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		mesa.render(glm::scale(mesa1, glm::vec3(3.0, 2.0, 0.08)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1 = glm::translate(mesa1, glm::vec3(1.5, -1, -0.5));
		pata1 = glm::rotate(pata1, glm::radians(90.0f), glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata1, glm::vec3(0.15, 1.0, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2 = glm::translate(mesa1, glm::vec3(-1.5, -1, -0.5));
		pata2 = glm::rotate(pata2, glm::radians(90.0f), glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata2, glm::vec3(0.15, 1.0, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3 = glm::translate(mesa1, glm::vec3(-1.5, 1, -0.5));
		pata3 = glm::rotate(pata3, glm::radians(90.0f), glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata3, glm::vec3(0.15, 1.0, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4 = glm::translate(mesa1, glm::vec3(1.5, 1, -0.5));
		pata4 = glm::rotate(pata4, glm::radians(90.0f), glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata4, glm::vec3(0.15, 1.0, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);




		// Posicion luz para objetos con materiales
		// Descomentar
		/*shaderMaterialLighting.setVectorFloat3("light.position",
		 glm::value_ptr(
		 glm::vec4(
		 lightModelmatrix
		 * glm::vec4(0.0, 0.0, 0.0, 1.0))));
		sphereLamp.render(lightModelmatrix);

		model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0, 1, 0));
		//box1.enableWireMode();
		//Descomentar
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		// Articulacion 1
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		j1 = glm::rotate(j1, rot1, glm::vec3(0, 0, 1));
		j1 = glm::rotate(j1, rot2, glm::vec3(0, 1, 0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));

		// Hueso 1
		glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0, 0.0));
		l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l1, glm::vec3(0.1, 0.5, 0.1)));

		// Articulacion 2
		glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0f, 0.0f));
		j2 = glm::rotate(j2, rot3, glm::vec3(0.0, 0.0, 1.0));
		j2 = glm::rotate(j2, rot4, glm::vec3(1.0, 0.0, 0.0));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

		// Hueso 2
		glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.5, 0.1)));

		// Ojo
		glm::mat4 ojo = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
		//sphere1.enableWireMode();
		sphere1.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));

		glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.05));
		//sphere2.enableWireMode();
		sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));

		glm::mat4 modelAgua = glm::mat4(1.0);
		modelAgua = glm::translate(modelAgua, glm::vec3(0.0, -3.0, 0.0));
		modelAgua = glm::scale(modelAgua, glm::vec3(5.0, 0.01, 5.0));
		// Se activa la textura del agua
		glBindTexture(GL_TEXTURE_2D, textureID2);
		shaderTexture.setFloat("offsetX", offX);
		box2.render(modelAgua);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderTexture.setFloat("offsetX", 0);

		glm::mat4 modelSphere = glm::mat4(1.0);
		modelSphere = glm::translate(modelSphere, glm::vec3(3.0, 0.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		sphere3.render(modelSphere);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 modelCylinder = glm::mat4(1.0);
		modelCylinder = glm::translate(modelCylinder,
				glm::vec3(-3.0, 0.0, 0.0));
		// Envolvente desde el indice 0, el tamanio es 20 * 20 * 6
		// Se usa la textura 1 ( Bon sponja)
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder2.render(0, cylinder2.getSlices() * cylinder2.getStacks() * 6,
				modelCylinder);
		// Tapa Superior desde el indice : 20 * 20 * 6, el tamanio de indices es 20 * 3
		// Se usa la textura 2 ( Agua )
		glBindTexture(GL_TEXTURE_2D, textureID2);
		cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 6,
				cylinder2.getSlices() * 3, modelCylinder);
		// Tapa inferior desde el indice : 20 * 20 * 6 + 20 * 3, el tamanio de indices es 20 * 3
		// Se usa la textura 3 ( Goku )
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(
				cylinder2.getSlices() * cylinder2.getStacks() * 6
						+ cylinder2.getSlices() * 3, cylinder2.getSlices() * 3,
				modelCylinder);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Render del cubo con textura de ladrillos y con repeticion en x
		glm::mat4 cubeTextureModel = glm::mat4(1.0);
		cubeTextureModel = glm::translate(cubeTextureModel,
				glm::vec3(3.0, 2.0, 3.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV",
				glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(cubeTextureModel);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Render del cyindro con materiales
		// Descomentar
		glm::mat4 cylinderMaterialModel = glm::mat4(1.0);
		 cylinderMaterialModel = glm::translate(cylinderMaterialModel,  glm::vec3(3.0, 2.0, -3.0));
		 shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.24725f, 0.1995f, 0.0745f)));
		 shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.75164f, 0.60648f, 0.22648f)));
		 shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.628281f, 0.555802f, 0.366065f)));
		 shaderMaterialLighting.setFloat("material.shininess", 51.2f);
		 cylinderMaterials.render(cylinderMaterialModel);

		 glm::mat4 boxMaterialModel = glm::mat4(1.0f);
		 boxMaterialModel = glm::translate(boxMaterialModel, glm::vec3(-3.0, 2.0, -3.0));
		 shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.0215f, 0.1745f, 0.0215f)));
		 shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.07568f, 0.61424f, 0.07568f)));
		 shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.633f, 0.727811f, 0.633f)));
		 shaderMaterialLighting.setFloat("material.shininess", 76.8f);
		 boxMaterials.render(boxMaterialModel);

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001; */

		// Descomentar
		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		//Se deshabilita el modo delrecorte de cras ocultas para ver la esfera desde dentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode); 

		dz = 0;
		rot0 = 0;
		offX += 0.001;

		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
