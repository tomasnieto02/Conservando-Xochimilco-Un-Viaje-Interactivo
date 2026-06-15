/*
Final project for the Computer Graphics and Computer-Human Interaction course at the National Autonomous University of Mexico (UNAM)

Authors:
Gómez Luna Brisa Mariana

Hernández Díaz Osvaldo

Marentes Degollado Ian Paul

Nieto Rodríguez Tomás Andrés
*/

#include <iostream>
#include <stdlib.h>

// GLAD: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator
// https://glad.dav1d.de/
#include <glad/glad.h>

// GLFW: https://www.glfw.org/
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

#include <irrKlang.h>
using namespace irrklang;

// Functions
bool Start();
bool Update();

// Definition of callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


// Globals
GLFWwindow* window;

// Screen size (pixels)
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera definition (position in XYZ axis)
Camera camera(glm::vec3(210.75f, -4.3073f, 108.293f), 
	glm::vec3(0.0f, 1.0f, 0.0f), -185.0f, -10.0f);





Camera camera3rd(glm::vec3(0.0f, 0.0f, 0.0f));

// Controller for mouse movement
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Variables for animation play speed 

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;

glm::vec3 position(0.0f,0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 1.5f;
float     scaleV = 0.025f;
float     rotateCharacter = 0.0f;
float	  door_offset = 0.0f;
float door_rotation = 0.0f;
float fan1_rotation = 0.0f;         
bool fan1_rotate = false;          
glm::vec3 door_position(119.45f, -3.67f, 112.79f);
glm::vec3 fan1_position(113.129f, -0.979997f, 85.0141f);  
glm::vec3 fan2_position(90.6842f, -0.979997f, 105.488f);  
float fan2_rotation = 0.0f;
bool fan2_rotate = false;
glm::vec3 water_position(0.0f, -9.3f, 0.0f);
float water_scale = 1.0f;
glm::vec3 door_bath_position(84.1993f, -5.1f, 127.299f);
float door_bath_rotation = 0.0f;
bool door_bath_opening_in = false;
bool door_bath_opening_out = false;

glm::vec3 trajinera_position(109.2f, -4.09f, 90.5001f); 
float trajinera_rotation = 0.0f;
bool trajinera_moving_forward = false;


bool show_book_info = false;
float popup_height = 0.0f;
glm::vec3 book_position(81.9002f, -4.3f, 106.1f);

// Shaders
Shader *mLightsShader;
Shader *proceduralShader;
Shader *wavesShader;

Shader *cubemapShader;
Shader *dynamicShader;

// Loads model data
Model	*house;
Model   *door;
Model   *moon;
Model   *gridMesh;
Model* fan1;  
Model* fan2;
Model* door_bathroom;
Model* trajinera;
Model* book_model;
Model* ghost_model;


// Animated models
AnimatedModel   *character01;

float tradius = 10.0f;
float theta = 0.0f;
float alpha = 0.0f;

// Cubemap
CubeMap *mainCubeMap;

// Light gLight;
std::vector<Light> gLights;

// Materials
Material material01;

float proceduralTime = 0.0f;
float wavesTime = 0.0f;
float fan_rotation = 0.0f;
bool fan_rotate = false;


// Audio
ISoundEngine *SoundEngine = createIrrKlangDevice();

// Camera selection
bool    activeCamera = 1; 

// Main function
int main()
{
	if (!Start())
		return -1;

	//Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}

bool Start() {
	// GLFW initialization

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window creation with GLFW
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: Load all pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Depth buffer activation
	glEnable(GL_DEPTH_TEST);

	// Shader compilation and linkage
	mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	proceduralShader = new Shader("shaders/12_ProceduralAnimation.vs", "shaders/12_ProceduralAnimation.fs");
	wavesShader = new Shader("shaders/13_wavesAnimation.vs", "shaders/13_wavesAnimation.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");

	// Max rigging bones: 100
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	house = new Model("models/IllumModels/facha.fbx");
	door = new Model("models/IllumModels/di.fbx");
	fan1 = new Model("models/IllumModels/ventilador.fbx");
	fan2 = new Model("models/IllumModels/ventilador2.fbx");
	gridMesh = new Model("models/IllumModels/grid.fbx");
	door_bathroom = new Model("models/IllumModels/puerta2.fbx");
	trajinera = new Model("models/IllumModels/trajinera.fbx");
	ghost_model = new Model("models/IllumModels/fantasma.fbx");
	book_model = new Model("models/IllumModels/libro.fbx"); // 


	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/02/posx.jpg",
		"textures/cubemap/02/negx.jpg",
		"textures/cubemap/02/posy.jpg",
		"textures/cubemap/02/negy.jpg",
		"textures/cubemap/02/posz.jpg",
		"textures/cubemap/02/negz.jpg"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = forwardView;

	// Main light source
	Light light01;
	light01.Position = glm::vec3(70.0f, 30.0f, 70.0f);  
	light01.Color = glm::vec4(1.0f, 0.6f, 0.2f, 1.0f);  
	light01.Power = glm::vec4(7.0f);
	light01.Direction = glm::vec3(-1.0f, -0.5f, -1.0f);
	gLights.push_back(light01);

	// Secondary light source 1 (blue)
	Light light02;
	light02.Position = glm::vec3(-50.0f, 25.0f, -40.0f);
	light02.Color = glm::vec4(0.3f, 0.5f, 1.0f, 1.0f);  
	light02.Power = glm::vec4(3.0f);
	light02.Direction = glm::vec3(1.0f, -0.3f, 1.0f);
	gLights.push_back(light02);

	// Secondary light source 2 (violet)
	Light light03;
	light03.Position = glm::vec3(0.0f, 20.0f, 80.0f);
	light03.Color = glm::vec4(0.6f, 0.3f, 1.0f, 1.0f);
	light03.Power = glm::vec4(2.5f);
	light03.Direction = glm::vec3(0.0f, -1.0f, -1.0f);
	gLights.push_back(light03);

	// Secondary light source 3 (overhead light)
	Light light04;
	light04.Position = glm::vec3(0.0f, 100.0f, 0.0f);
	light04.Color = glm::vec4(1.0f, 0.95f, 0.9f, 1.0f);  
	light04.Power = glm::vec4(1.2f);
	light04.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	gLights.push_back(light04);
	Light museumLight;
	museumLight.Position = glm::vec3(5.0f, 5.0f, -3.0f); 
	museumLight.Color = glm::vec4(1.0f); 
	museumLight.Power = glm::vec4(2.0f); 
	museumLight.Direction = glm::vec3(0.0f, -1.0f, 0.0f); 
	gLights.push_back(museumLight);
	// Secondary light source 4 (museum entrance)
	Light entryLight;
	entryLight.Position = glm::vec3(3.5f, 2.5f, 5.5f); 
	entryLight.Color = glm::vec4(1.0f);
	entryLight.Power = glm::vec4(2.0f);
	entryLight.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	entryLight.Power = glm::vec4(20.0f);
	 SoundEngine->play2D("sound/audio.mp3", true);
	return true;
}



void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec3(uniformName.c_str(), value);
}


bool Update() {
	// Framerate calculation
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Input processing
	processInput(window);

	// Renderer R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;

	if (activeCamera) {
		// First person camera
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera.GetViewMatrix();
	}
	else {
		// Third person camera
		projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera3rd.GetViewMatrix();
	}

	// Cubemap (background)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}
	
	{
		mLightsShader->use();

		// Transparent objects shader
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mLightsShader->setMat4("projection", projection);
		mLightsShader->setMat4("view", view);

		// Model transformations
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));// translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		mLightsShader->setMat4("model", model);

		// Light source property configuration
		mLightsShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(mLightsShader, "Color", i, gLights[i].Color);
			SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
		}

		mLightsShader->setVec3("eye", camera.Position);

		// Material property shaders
		mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
		mLightsShader->setFloat("transparency", material01.transparency);

		house->Draw(*mLightsShader);

		model = glm::mat4(1.0f);

		// Door model animations
		model = glm::mat4(1.0f);
		model = glm::translate(model, door_position); 
		model = glm::rotate(model, glm::radians(door_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		door->Draw(*mLightsShader);

		// Spinning fan model animation
		glm::mat4 model_fan = glm::mat4(1.0f);
		model_fan = glm::translate(model_fan, fan1_position); 
		model_fan = glm::rotate(model_fan, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model_fan = glm::rotate(model_fan, glm::radians(fan_rotation), glm::vec3(0.0f, 0.0f, 1.0f)); 

		mLightsShader->setMat4("model", model_fan);
		fan1->Draw(*mLightsShader);
		// Update rotation if active
		if (fan_rotate) {
			fan_rotation += 460.0f * deltaTime;
		}
		// Spinning fan 2 model animation
		model = glm::mat4(1.0f);
		model = glm::translate(model, fan2_position);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(fan2_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		mLightsShader->setMat4("model", model);
		fan2->Draw(*mLightsShader);

		// Update rotation if active (fan 2)
		if (fan2_rotate) {
			fan2_rotation += 100.0f * deltaTime;
		}

		float dist = glm::distance(camera.Position, door_bath_position);

		// Open door if near
		if (dist < 10.0f && door_bath_rotation < 100.0f) {
			door_bath_rotation += 60.0f * deltaTime;
		}
		// Close door if far
		else if (dist >= 10.0f && door_bath_rotation > 0.0f) {
			door_bath_rotation -= 60.0f * deltaTime;
		}

		// Render lavatory door
		glm::mat4 model_bath = glm::mat4(1.0f);
		model_bath = glm::translate(model_bath, door_bath_position);
		model_bath = glm::rotate(model_bath, glm::radians(door_bath_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		model_bath = glm::rotate(model_bath, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model_bath);
		door_bathroom->Draw(*mLightsShader);

		// ----------------------
    // Trajinera animation
	float dist_trajinera = glm::distance(camera.Position, trajinera_position);

		if (dist_trajinera < 10.0f) {
			trajinera_rotation += 60.0f * deltaTime;  
			if (trajinera_rotation > 360.0f) trajinera_rotation -= 360.0f;
		}

		// Render trajinera
		glm::mat4 model_trajinera = glm::mat4(1.0f);
		model_trajinera = glm::translate(model_trajinera, trajinera_position);
		model_trajinera = glm::rotate(model_trajinera, glm::radians(trajinera_rotation), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model_trajinera = glm::rotate(model_trajinera, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
		mLightsShader->setMat4("model", model_trajinera);
		trajinera->Draw(*mLightsShader);

		
		// Falling book animation
		
		glm::mat4 model_book = glm::mat4(1.0f);
		model_book = glm::translate(model_book, book_position);
		model_book = glm::rotate(model_book, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model_book);
		book_model->Draw(*mLightsShader);

		// Pop up poster animation
		if (show_book_info) {
			popup_height += deltaTime * 1.5f;
			if (popup_height > 1.5f) popup_height = 1.5f;

			glm::vec3 ghost_pos = book_position + glm::vec3(0.0f, popup_height + 1.0f, 0.0f);
			glm::mat4 ghost_model_mat = glm::mat4(1.0f);
			ghost_model_mat = glm::translate(ghost_model_mat, ghost_pos);
			ghost_model_mat = glm::rotate(ghost_model_mat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ghost_model_mat = glm::scale(ghost_model_mat, glm::vec3(0.45f));  

			mLightsShader->setMat4("model", ghost_model_mat);
			ghost_model->Draw(*mLightsShader);
		}





	}

	glUseProgram(0);


	{
		wavesShader->use();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		wavesShader->setMat4("projection", projection);
		wavesShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, water_position);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(water_scale, water_scale, water_scale)); 
		wavesShader->setMat4("model", model);

		wavesShader->setFloat("time", wavesTime);
		wavesShader->setFloat("radius", 5.0f);
		wavesShader->setFloat("height", 5.0f);

		gridMesh->Draw(*wavesShader);
		wavesTime += 0.0003;
	}


	glUseProgram(0);
	


	glUseProgram(0); 

	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Keyboard input processing
void processInput(GLFWwindow* window)
{
	camera.MovementSpeed = 15.0f;  

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		door_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		door_offset -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		door_rotation += 1.f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		door_rotation -= 1.f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.Position.y += deltaTime * camera.MovementSpeed;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.Position.y -= deltaTime * camera.MovementSpeed;
	

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		activeCamera = 0;
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		activeCamera = 1;
	
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		fan_rotate = true;
		fan2_rotate = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		fan_rotate = false;
		fan2_rotate = false;
	}
	
	
}

// glfw: Updates viewport if window size changes

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Movement and mouse event callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; 

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: Add-on for mouse movement and events
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

// Click callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;
		glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

		glm::vec3 ray_world = glm::vec3(glm::inverse(view) * ray_eye);
		ray_world = glm::normalize(ray_world);

		glm::vec3 ray_origin = camera.Position;
		glm::vec3 ray_direction = ray_world;

		glm::vec3 minBB = book_position + glm::vec3(-0.5f, -0.5f, -0.5f);
		glm::vec3 maxBB = book_position + glm::vec3(0.5f, 0.5f, 0.5f);

		float tMin = (minBB.x - ray_origin.x) / ray_direction.x;
		float tMax = (maxBB.x - ray_origin.x) / ray_direction.x;
		if (tMin > tMax) std::swap(tMin, tMax);

		float tyMin = (minBB.y - ray_origin.y) / ray_direction.y;
		float tyMax = (maxBB.y - ray_origin.y) / ray_direction.y;
		if (tyMin > tyMax) std::swap(tyMin, tyMax);

		if ((tMin > tyMax) || (tyMin > tMax)) return;

		tMin = std::max(tMin, tyMin);
		tMax = std::min(tMax, tyMax);

		float tzMin = (minBB.z - ray_origin.z) / ray_direction.z;
		float tzMax = (maxBB.z - ray_origin.z) / ray_direction.z;
		if (tzMin > tzMax) std::swap(tzMin, tzMax);

		if ((tMin > tzMax) || (tzMin > tMax)) return;

		show_book_info = true;
popup_height = 0.0f;

		std::cout << "¡Libro activado por clic!" << std::endl;
	}
}
