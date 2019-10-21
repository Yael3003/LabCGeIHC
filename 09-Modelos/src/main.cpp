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

#include "Headers/Texture.h"

//include loader model class
#include "Headers/Model.h"

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
// Shader con multiples luces
Shader shaderMulLighting;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Sphere sphereLamp(20, 20);
// Descomentar
Sphere skyboxSphere(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Cylinder cylinderMaterials(20, 20, 0.5, 0.5);
Cylinder pata(20, 20, 0.5, 0.5), pataSilla(20, 20, 0.5, 0.5);
Box boxMaterials;
Box box1;
Box box2;
Box box3;
Box pared1, paredBanio, piso1, piso2, techo1;
//Para los sofas
Box sofabk, sofac1, sofac2, sofac3, sofac4, sofac5, sofal, sofar;
//Para mesa y muebles 
Box mesa, asientoSilla, respaldoSilla, mueble1Sup, mueble1Inf, muebleCocina;
// Models complex instances
//Model modelRock; 
//Model modelRailRoad;
//Model modelAircraft;
Model modelTree;
Model modelToilet;
//Model modelSofa;

GLuint textureID1, textureID2, textureID3, textureID4, textureIDCocina, 
textureIDEstufa, textureIDEstufaSup, textureIDRefri, textureIDRefriSup, textureIDAzulejos, textureIDPasto;
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
	shaderMaterialLighting.initialize("../Shaders/iluminacion_material.vs",
		"../Shaders/iluminacion_material_res.fs");
	shaderSkybox.initialize("../Shaders/cubeTexture.vs",
		"../Shaders/cubeTexture.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_texture_res.vs",
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

	cylinder1.init();
	cylinder1.setShader(&shaderColorLighting);
	cylinder1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderMulLighting); //este solo trabaja con texturas, no color

	cylinderMaterials.init();
	cylinderMaterials.setShader(&shaderMaterialLighting);
	cylinderMaterials.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	boxMaterials.init();
	boxMaterials.setShader(&shaderMaterialLighting);

	// Inicializacion de la esfera del skybox
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	box1.init();
	// Settea el shader a utilizar 
	box1.setShader(&shaderMulLighting);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	box2.init();
	box2.setShader(&shaderMulLighting);

	sphere3.init();
	sphere3.setShader(&shaderMulLighting);

	box3.init();
	box3.setShader(&shaderMulLighting);

	pared1.init();
	// Settea el shader a utilizar
	pared1.setShader(&shader);
	pared1.setColor(glm::vec4(0.85, 0.8, 0.4, 1.0));

	paredBanio.init();
	// Settea el shader a utilizar
	paredBanio.setShader(&shaderTexture);
	paredBanio.setColor(glm::vec4(0.85, 0.8, 0.4, 1.0));

	piso1.init();
	// Settea el shader a utilizar
	piso1.setShader(&shaderTextureLighting);
	piso1.setColor(glm::vec4(0.3, 0.2, 0.25, 1.0));

	piso2.init();
	// Settea el shader a utilizar
	piso2.setShader(&shaderTextureLighting);
	piso2.setColor(glm::vec4(0.3, 0.2, 0.25, 1.0));

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

	pata.init();
	pata.setShader(&shader);
	pata.setColor(glm::vec4(0.12, 0.1, 0.15, 1.0));

	asientoSilla.init();
	asientoSilla.setShader(&shader);
	asientoSilla.setColor(glm::vec4(0.6, 0.5, 0.3, 1.0));

	pataSilla.init();
	pataSilla.setShader(&shader);
	pataSilla.setColor(glm::vec4(0.12, 0.1, 0.15, 1.0));

	respaldoSilla.init();
	respaldoSilla.setShader(&shader);
	respaldoSilla.setColor(glm::vec4(0.65, 0.55, 0.35, 1.0));

	muebleCocina.init();
	muebleCocina.setShader(&shaderTexture);
	muebleCocina.setColor(glm::vec4(0.85, 0.75, 0.55, 1.0));


	//modelRock.loadModel("../models/rock/rock.obj");
	//modelRock.setShader(&shaderMulLighting);

	//modelRock.loadModel("../models/railroad/railroad_track.obj");
	//modelRock.setShader(&shaderMulLighting);

	//modelAircraft.loadModel("../models/Aircraft_obj/E 45 Aircraft_obj.obj");
	//modelAircraft.setShader(&shaderMulLighting);

	modelTree.loadModel("../models/Tree/Christmas Tree.obj");
	modelTree.setShader(&shaderMulLighting);

	modelTree.loadModel("../models/Toilet/Toilet.1.obj");
	modelTree.setShader(&shaderMulLighting);

	//modelSofa.loadModel("../models/model_obj_7Q_Ab2HLll1_c7YDClm08KI/model.obj");
	//modelSofa.setShader(&shaderMulLighting);

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
	}
	else
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture2.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture3("../Textures/muebleCocina.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture3.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture3.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDCocina);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDCocina);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture3.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture4("../Textures/texturaEstufa.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture4.loadImage(false);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDEstufa);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDEstufa);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture4.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture5("../Textures/texturaEstufaSup.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture5.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture5.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDEstufaSup);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDEstufaSup);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture5.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture6("../Textures/texturaRefri.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture6.loadImage(false);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture5.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDRefri);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDRefri);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture6.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture7("../Textures/metal.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture7.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture7.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDRefriSup);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDRefriSup);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture7.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture8("../Textures/azulejos.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture8.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture8.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDAzulejos);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDAzulejos);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture8.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture9("../Textures/pasto.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture9.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture9.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureIDPasto);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureIDPasto);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture9.freeImage(bitmap);

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
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
		}
		else
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
	cylinder1.destroy();
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

	glm::mat4 model = glm::mat4(1.0f);
	float offX = 0.0;
	float angle = 0.0;
	float ratio = 5.0;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);
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
		shaderMaterialLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMaterialLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4((view))));

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
		shaderMaterialLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMaterialLighting.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMaterialLighting.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMaterialLighting.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));

		// Propiedades de la luz para objetos con multiples luces
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.7, 0.9, 0.9)));
		//Esto es para la luz spotLight
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(camera->getFront()));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(17.0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.1);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.05);



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

		// Posicion luz para objetos con materiales
		shaderMaterialLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));
		sphereLamp.render(lightModelmatrix);

		glm::mat4 piso = glm::mat4(1.0f);
		piso = glm::translate(piso, glm::vec3(0, 0, 0));
		//piso = glm::rotate(piso, glm::radians(90.0f), glm::vec3(1.0, 0, 0.0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		piso1.render(glm::scale(piso, glm::vec3(20.0, 0.1, 10)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pasto = glm::mat4(1.0f);
		pasto = glm::translate(piso, glm::vec3(0, 0, 10));
		// Usamos la textura ID 
		glBindTexture(GL_TEXTURE_2D, textureIDPasto);
		piso2.render(glm::scale(pasto, glm::vec3(20.0, 0.1, 10)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredT = glm::translate(piso, glm::vec3(0, 2.55, -4.95));
		paredT = glm::rotate(paredT, rot0, glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredT, glm::vec3(20.0, 5, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredCen = glm::translate(piso, glm::vec3(0, 2.55, -1.25));
		//paredCen = glm::rotate(paredCen, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredCen, glm::vec3(0.1, 5, 2.5)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredIzq = glm::mat4(1.0f);
		paredIzq = glm::translate(piso, glm::vec3(-9.95, 2.55, 0));
		//paredIzq = glm::rotate(paredIzq, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredIzq, glm::vec3(0.1, 5, 10)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredDer = glm::mat4(1.0f);
		paredDer = glm::translate(piso, glm::vec3(9.95, 2.55, 0));
		//paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredDer, glm::vec3(0.1, 5, 10)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredFrenteComedor = glm::mat4(1.0f);
		paredFrenteComedor = glm::translate(piso, glm::vec3(6.25, 2.55, 0));
		//paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredFrenteComedor, glm::vec3(7.5, 5, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredFrenteSala = glm::mat4(1.0f);
		paredFrenteSala = glm::translate(piso, glm::vec3(-6.25, 2.55, 1.2));
		//paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredFrenteSala, glm::vec3(7.5, 5, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredFrenteCocina = glm::mat4(1.0f);
		paredFrenteCocina = glm::translate(piso, glm::vec3(6.25, 2.55, 4.95));
		//paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredFrenteCocina, glm::vec3(7.5, 5, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 paredFrenteBanio = glm::mat4(1.0f);
		paredFrenteBanio = glm::translate(piso, glm::vec3(-6.25, 2.55, 4.95));
		//paredDer = glm::rotate(paredDer, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pared1.render(glm::scale(paredFrenteBanio, glm::vec3(7.5, 5, 0.1)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredFrenteBanioTx = glm::mat4(1.0f);
		paredFrenteBanioTx = glm::translate(paredFrenteBanio, glm::vec3(0, 0, -0.05));
		// Usamos la textura ID 
		glBindTexture(GL_TEXTURE_2D, textureIDAzulejos);
		paredBanio.render(glm::scale(paredFrenteBanioTx, glm::vec3(7.5, 5, 0.001)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 paredTraBanioTx = glm::mat4(1.0f);
		paredTraBanioTx = glm::translate(paredFrenteSala, glm::vec3(0, 0, 0.05));
		// Usamos la textura ID 
		glBindTexture(GL_TEXTURE_2D, textureIDAzulejos);
		paredBanio.render(glm::scale(paredTraBanioTx, glm::vec3(7.5, 5, 0.001)));
		//Descomentar
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaTras = glm::translate(piso, glm::vec3(-5, 1.05, -4.7));
		//sofaTras = glm::rotate(sofaTras, rot0, glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofaTras, glm::vec3(3.0, 2.0, 0.05)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaIzq = glm::translate(sofaTras, glm::vec3(-1.5, -0.3, 0.5));
		//sofaIzq = glm::rotate(sofaIzq, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofaIzq, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaDer = glm::translate(sofaTras, glm::vec3(1.5, -0.3, 0.5));
		//sofaDer = glm::rotate(sofaDer, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofaDer, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin1 = glm::translate(sofaTras, glm::vec3(0.0, -0.7, 0.625));
		//sofaCojin1 = glm::rotate(sofaCojin1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofaCojin1, glm::vec3(2.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin2 = glm::translate(sofaTras, glm::vec3(0.0, -0.3, 0.625));
		//sofaCojin2 = glm::rotate(sofaCojin2, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofaCojin2, glm::vec3(2.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin3 = glm::translate(sofaTras, glm::vec3(-0.95, 0.4, 0.1));
		//sofaCojin3 = glm::rotate(sofaCojin3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac3.render(glm::scale(sofaCojin3, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin4 = glm::translate(sofaTras, glm::vec3(0, 0.4, 0.1));
		//sofaCojin4 = glm::rotate(sofaCojin4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac4.render(glm::scale(sofaCojin4, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofaCojin5 = glm::translate(sofaTras, glm::vec3(0.95, 0.4, 0.1));
		//sofaCojin5 = glm::rotate(sofaCojin5, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac5.render(glm::scale(sofaCojin5, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//Segundo sofa--------------------------------------------------------

		glm::mat4 sofa2Tras = glm::translate(piso, glm::vec3(-5, 1.05, 1));
		sofa2Tras = glm::rotate(sofa2Tras, glm::radians(180.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofa2Tras, glm::vec3(2.0, 2.0, 0.05)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Izq = glm::translate(sofa2Tras, glm::vec3(-1, -0.3, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofa2Izq, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Der = glm::translate(sofa2Tras, glm::vec3(1, -0.3, 0.5));
		//sofaDer = glm::rotate(sofaDer, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofa2Der, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin1 = glm::translate(sofa2Tras, glm::vec3(0.0, -0.7, 0.625));
		//sofaCojin1 = glm::rotate(sofaCojin1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofa2Cojin1, glm::vec3(1.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin2 = glm::translate(sofa2Tras, glm::vec3(0.0, -0.3, 0.625));
		//sofaCojin2 = glm::rotate(sofaCojin2, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofa2Cojin2, glm::vec3(1.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin3 = glm::translate(sofa2Tras, glm::vec3(-0.45, 0.4, 0.15));
		//sofaCojin3 = glm::rotate(sofaCojin3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac3.render(glm::scale(sofa2Cojin3, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa2Cojin4 = glm::translate(sofa2Tras, glm::vec3(0.45, 0.4, 0.15));
		//sofaCojin4 = glm::rotate(sofaCojin4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac4.render(glm::scale(sofa2Cojin4, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//----------------Tercer sofa-------------------------------------------

		glm::mat4 sofa3Tras = glm::translate(piso, glm::vec3(-1.2, 1.05, -1.5));
		sofa3Tras = glm::rotate(sofa3Tras, glm::radians(270.0f), glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofabk.render(glm::scale(sofa3Tras, glm::vec3(1, 2, 0.05)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Izq = glm::translate(sofa3Tras, glm::vec3(-0.5, -0.3, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofal.render(glm::scale(sofa3Izq, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura             
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Der = glm::translate(sofa3Tras, glm::vec3(0.5, -0.3, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofar.render(glm::scale(sofa3Der, glm::vec3(0.05, 1.4, 1.0)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin1 = glm::translate(sofa3Tras, glm::vec3(0.0, -0.3, 0.625));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac1.render(glm::scale(sofa3Cojin1, glm::vec3(0.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin2 = glm::translate(sofa3Tras, glm::vec3(0.0, -0.7, 0.625));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac2.render(glm::scale(sofa3Cojin2, glm::vec3(0.9, 0.4, 1.2)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 sofa3Cojin3 = glm::translate(sofa3Tras, glm::vec3(0, 0.4, 0.125));
		//sofaCojin4 = glm::rotate(sofaCojin4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		sofac4.render(glm::scale(sofa3Cojin3, glm::vec3(0.9, 1, 0.25)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//--------------------------------------MESA-------------------------------------------------

		glm::mat4 mesa1 = glm::translate(piso, glm::vec3(5, 1.5, -2.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		mesa.render(glm::scale(mesa1, glm::vec3(3, 0.08, 1.5)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1 = glm::translate(mesa1, glm::vec3(-1.45, -0.75, -0.7));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata1, glm::vec3(0.15, 1.42, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2 = glm::translate(mesa1, glm::vec3(-1.45, -0.75, 0.7));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata2, glm::vec3(0.15, 1.42, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3 = glm::translate(mesa1, glm::vec3(1.45, -0.75, -0.7));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata3, glm::vec3(0.15, 1.42, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4 = glm::translate(mesa1, glm::vec3(1.45, -0.75, 0.7));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pata.render(glm::scale(pata4, glm::vec3(0.15, 1.42, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		//-----------------------------------SILLAS---------------------------------------------

		glm::mat4 asientoSilla1 = glm::translate(mesa1, glm::vec3(0.75, -0.6, -1.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		asientoSilla.render(glm::scale(asientoSilla1, glm::vec3(1, 0.1, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1Silla1 = glm::translate(asientoSilla1, glm::vec3(0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata1Silla1, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2Silla1 = glm::translate(asientoSilla1, glm::vec3(-0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata2Silla1, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3Silla1 = glm::translate(asientoSilla1, glm::vec3(0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata3Silla1, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4Silla1 = glm::translate(asientoSilla1, glm::vec3(-0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata4Silla1, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 respaldoSilla1 = glm::translate(asientoSilla1, glm::vec3(0.0, 0.675, -0.45));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		respaldoSilla.render(glm::scale(respaldoSilla1, glm::vec3(1, 1.25, 0.075)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 asientoSilla2 = glm::translate(mesa1, glm::vec3(-0.75, -0.6, -1.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		asientoSilla.render(glm::scale(asientoSilla2, glm::vec3(1, 0.1, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1Silla2 = glm::translate(asientoSilla2, glm::vec3(0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata1Silla2, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2Silla2 = glm::translate(asientoSilla2, glm::vec3(-0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata2Silla2, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3Silla2 = glm::translate(asientoSilla2, glm::vec3(0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata3Silla2, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4Silla2 = glm::translate(asientoSilla2, glm::vec3(-0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata4Silla2, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 respaldoSilla2 = glm::translate(asientoSilla2, glm::vec3(0.0, 0.675, -0.45));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		respaldoSilla.render(glm::scale(respaldoSilla2, glm::vec3(1, 1.25, 0.075)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 asientoSilla3 = glm::translate(mesa1, glm::vec3(0.75, -0.6, 1.5));
		asientoSilla3 = glm::rotate(asientoSilla3, glm::radians(180.0f), glm::vec3(0, 1.0, 0.0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		asientoSilla.render(glm::scale(asientoSilla3, glm::vec3(1, 0.1, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1Silla3 = glm::translate(asientoSilla3, glm::vec3(0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata1Silla3, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2Silla3 = glm::translate(asientoSilla3, glm::vec3(-0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata2Silla3, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3Silla3 = glm::translate(asientoSilla3, glm::vec3(0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata3Silla3, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4Silla3 = glm::translate(asientoSilla3, glm::vec3(-0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata4Silla3, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 respaldoSilla3 = glm::translate(asientoSilla3, glm::vec3(0.0, 0.675, -0.45));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		respaldoSilla.render(glm::scale(respaldoSilla3, glm::vec3(1, 1.25, 0.075)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 asientoSilla4 = glm::translate(mesa1, glm::vec3(-0.75, -0.6, 1.5));
		asientoSilla4 = glm::rotate(asientoSilla4, glm::radians(180.0f), glm::vec3(0, 1.0, 0.0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		asientoSilla.render(glm::scale(asientoSilla4, glm::vec3(1, 0.1, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1Silla4 = glm::translate(asientoSilla4, glm::vec3(0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata1Silla4, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2Silla4 = glm::translate(asientoSilla4, glm::vec3(-0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata2Silla4, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3Silla4 = glm::translate(asientoSilla4, glm::vec3(0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata3Silla4, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4Silla4 = glm::translate(asientoSilla4, glm::vec3(-0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata4Silla4, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 respaldoSilla4 = glm::translate(asientoSilla4, glm::vec3(0.0, 0.675, -0.45));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		respaldoSilla.render(glm::scale(respaldoSilla4, glm::vec3(1, 1.25, 0.075)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		glm::mat4 asientoSilla5 = glm::translate(mesa1, glm::vec3(2.2, -0.6, 0));
		asientoSilla5 = glm::rotate(asientoSilla5, glm::radians(270.0f), glm::vec3(0, 1.0, 0.0));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		asientoSilla.render(glm::scale(asientoSilla5, glm::vec3(1, 0.1, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata1Silla5 = glm::translate(asientoSilla5, glm::vec3(0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata1Silla5, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata2Silla5 = glm::translate(asientoSilla5, glm::vec3(-0.5, -0.45, 0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata2Silla5, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata3Silla5 = glm::translate(asientoSilla5, glm::vec3(0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata3Silla5, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 pata4Silla5 = glm::translate(asientoSilla5, glm::vec3(-0.5, -0.45, -0.5));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		pataSilla.render(glm::scale(pata4Silla5, glm::vec3(0.15, 0.85, 0.15)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 respaldoSilla5 = glm::translate(asientoSilla5, glm::vec3(0.0, 0.675, -0.45));
		// Usamos la textura ID 1
		//glBindTexture(GL_TEXTURE_2D, textureID1);
		respaldoSilla.render(glm::scale(respaldoSilla5, glm::vec3(1, 1.25, 0.075)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		//------------------------------------------Cocina-------------------------------------------
		
		glm::mat4 muebleCocina1 = glm::translate(paredFrenteComedor, glm::vec3(-2, -1.5, 0.6));
		muebleCocina1 = glm::rotate(muebleCocina1, glm::radians(180.0f), glm::vec3(0, 1.0, 0.0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDCocina);
		muebleCocina.render(glm::scale(muebleCocina1, glm::vec3(3, 2, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 estufa = glm::translate(paredFrenteComedor, glm::vec3(0.5, -1.5, 0.6));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDEstufa);
		muebleCocina.render(glm::scale(estufa, glm::vec3(2, 2, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 estufaSup = glm::translate(estufa, glm::vec3(0, 1, 0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDEstufaSup);
		muebleCocina.render(glm::scale(estufaSup, glm::vec3(2, 0.01, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 muebleCocina2 = glm::translate(paredFrenteComedor, glm::vec3(2.5, -1.5, 0.6));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDCocina);
		muebleCocina.render(glm::scale(muebleCocina2, glm::vec3(2, 2, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 muebleCocina3 = glm::translate(paredFrenteComedor, glm::vec3(-1, -1.5, 4.4));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDCocina);
		muebleCocina.render(glm::scale(muebleCocina3, glm::vec3(3, 2, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 refrigerador= glm::translate(paredFrenteComedor, glm::vec3(1.5, -0.5, 4.4));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDRefri);
		muebleCocina.render(glm::scale(refrigerador, glm::vec3(2, 4, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 refrigeradorSup = glm::translate(refrigerador, glm::vec3(0, 2, 0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDRefriSup);
		muebleCocina.render(glm::scale(refrigeradorSup, glm::vec3(2, 0.01, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 refrigeradorDer = glm::translate(refrigerador, glm::vec3(1, 0, 0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDRefriSup);
		muebleCocina.render(glm::scale(refrigeradorDer, glm::vec3(0.01, 4, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 refrigeradorIzq = glm::translate(refrigerador, glm::vec3(-1, 0, 0));
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureIDRefriSup);
		muebleCocina.render(glm::scale(refrigeradorIzq, glm::vec3(0.01, 4, 1)));
		// No utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);


		/*model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0, 1, 0));
		//box1.enableWireMode();
		// Usamos la textura ID 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
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
		//le cambiamos el shader con multiplesluces NO OLVIDAR
		shaderMulLighting.setFloat("offsetX", offX);
		box2.render(modelAgua);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderMulLighting.setFloat("offsetX", 0);

		glm::mat4 modelSphere = glm::mat4(1.0);
		modelSphere = glm::translate(modelSphere, glm::vec3(3.0, 0.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		sphere3.render(modelSphere);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 modelCylinder = glm::mat4(1.0);
		modelCylinder = glm::translate(modelCylinder,
			glm::vec3(-3.0, 0.0, 0.0));
		// Envolvente desde el indice 0, el tamanio es 20 * 20 * 6
		// Se usa la textura 1 ( Bob sponja)
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
		/*glm::mat4 cubeTextureModel = glm::mat4(1.0);
		cubeTextureModel = glm::translate(cubeTextureModel,
			glm::vec3(3.0, 2.0, 3.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV",
			glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(cubeTextureModel);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Render del cyindro con materiales
		glm::mat4 cylinderMaterialModel = glm::mat4(1.0);
		cylinderMaterialModel = glm::translate(cylinderMaterialModel, glm::vec3(3.0, 2.0, -3.0));
		shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
		shaderMaterialLighting.setFloat("material.shininess", 76.8f);
		cylinderMaterials.render(cylinderMaterialModel);

		/*glm::mat4 boxMaterialModel = glm::mat4(1.0f);
		boxMaterialModel = glm::translate(boxMaterialModel, glm::vec3(-3.0, 2.0, -3.0));
		shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
		shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
		shaderMaterialLighting.setFloat("material.shininess", 76.8f);
		boxMaterials.render(boxMaterialModel);
		//Models complex render
		glm::mat4 matrixModelRock = glm::mat4(1.0);
		matrixModelRock = glm::translate(matrixModelRock, glm::vec3(-3.0, 0.0, 4.0));
		modelRock.render(matrixModelRock);
		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);
		//Esto es para las vias del tren 
		glm::mat4 matrixModelRailRoad = glm::mat4(1.0);
		matrixModelRock = glm::translate(matrixModelRailRoad, glm::vec3(3.0, 0.0, 4.0));
		modelRailRoad.render(matrixModelRailRoad);
		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);
		//Esto es para el modelo Aircraft
		glm::mat4 matrixModelAircraft = glm::mat4(1.0);
		matrixModelAircraft = glm::translate(matrixModelAircraft, glm::vec3(3.0, 0.0, 0.0));
		modelAircraft.render(matrixModelAircraft);
		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);*/
		//Esto es para el modelo Tree
		glm::mat4 matrixModelTree = glm::mat4(1.0);
		matrixModelTree = glm::translate(matrixModelTree, glm::vec3(5.0, 0.05, 11));
		matrixModelTree = glm::scale(matrixModelTree, glm::vec3(0.2, 0.2, 0.2));
		modelTree.render(matrixModelTree);
		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		glm::mat4 matrixModelToilet = glm::mat4(1.0);
		matrixModelToilet = glm::translate(matrixModelToilet, glm::vec3(-8.5, 0.05, 2.5));
		modelToilet.render(matrixModelToilet);

		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);
		/*Esto es para el modelo Sofa
		glm::mat4 matrixModelSofa = glm::mat4(1.0);
		matrixModelSofa = glm::translate(matrixModelSofa, glm::vec3(5.0, 3.0, 0.0));
		//matrixModelSofa = glm::scale(matrixModelSofa, glm::vec3(0.2, 0.2, 0.2));
		modelSofa.render(matrixModelSofa);
		//Forze to enable the unit texture to 0 always-------------------------IMPORTANT
		glActiveTexture(GL_TEXTURE0);*/

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
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