#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char* l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;

	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 20.0f;
	scaleFactor = 1.0f;
	lightAngle = 0.0f;
	textureBlend = 0.0f;

	useDiffuse = false;

	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();
	myObjectPLY = new ply("./data/sphere.ply");
	myEnvironmentPLY = new ply("./data/sphere.ply");
}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete myObjectPLY;
	delete myEnvironmentPLY;
}

void MyGLCanvas::initShaders() {
	myTextureManager->loadTexture("environMap", "./data/sphere-map-market.ppm");
	myTextureManager->loadTexture("objectTexture", "./data/brick.ppm");

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->buildArrays();
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);
}



void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		if (firstTime == true) {
			firstTime = false;
			initShaders();
		}
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
}

void MyGLCanvas::drawScene() {
	glm::mat4 viewMatrix = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));

	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelMatrix = glm::mat4(1.0);
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

	glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	//Pass first texture info to our shader 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("environMap"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("objectTexture"));

	//first draw the object sphere
	glUseProgram(myShaderManager->getShaderProgram("objectShaders")->programID);

	//TODO: add variable binding
	GLuint objectShaderProgram = myShaderManager->getShaderProgram("objectShaders")->programID;

	// Get uniform locations
	GLint modelLoc = glGetUniformLocation(objectShaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(objectShaderProgram, "view");
	GLint projLoc = glGetUniformLocation(objectShaderProgram, "projection");
	GLint lightPosLoc = glGetUniformLocation(objectShaderProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(objectShaderProgram, "viewPos");
	GLint textureBlendLoc = glGetUniformLocation(objectShaderProgram, "textureBlend");
	GLint useDiffuseLoc = glGetUniformLocation(objectShaderProgram, "useDiffuse");

	// Pass matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

	// Pass other uniforms
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glm::vec4 rotatedEye = glm::inverse(viewMatrix) * glm::vec4(eyePosition, 1.0f);
	glm::vec3 transformedEye = glm::vec3(rotatedEye);
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(transformedEye));
	glUniform1f(textureBlendLoc, textureBlend);
	glUniform1i(useDiffuseLoc, useDiffuse);

	// Pass texture units
	GLint environMapLoc = glGetUniformLocation(objectShaderProgram, "environMap");
	GLint objectTextureLoc = glGetUniformLocation(objectShaderProgram, "objectTexture");
	glUniform1i(environMapLoc, 0);  // GL_TEXTURE0
	glUniform1i(objectTextureLoc, 1);  // GL_TEXTURE1


	myObjectPLY->renderVBO(myShaderManager->getShaderProgram("objectShaders")->programID);



	//second draw the enviroment sphere
	glUseProgram(myShaderManager->getShaderProgram("environmentShaders")->programID);

	// Get shader program
	GLuint environmentShaderProgram = myShaderManager->getShaderProgram("environmentShaders")->programID;

	// Variable binding for environment shader
	GLint envModelLoc = glGetUniformLocation(environmentShaderProgram, "model");
	GLint envViewLoc = glGetUniformLocation(environmentShaderProgram, "view");
	GLint envProjLoc = glGetUniformLocation(environmentShaderProgram, "projection");

	// Create environment model matrix (scaled up)
	glm::mat4 environmentModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(7.0f, 7.0f, 7.0f));

	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(envModelLoc, 1, GL_FALSE, glm::value_ptr(environmentModelMatrix));
	glUniformMatrix4fv(envViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(envProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

	// Pass texture unit for environment shader
	GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "environMap");
	glUniform1i(environMapLocEnv, 0);  // GL_TEXTURE0

	myEnvironmentPLY->renderVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	perspectiveMatrix = glm::perspective(TO_RADIANS(viewAngle), xy_aspect, clipNear, clipFar);
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err) {
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		else {
			//SHADER: initialize the shader manager and loads the two shader programs
			initShaders();
		}
	}
#endif	
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_DRAG:
	case FL_MOVE:
	case FL_PUSH:
	case FL_RELEASE:
	case FL_KEYUP:
	case FL_MOUSEWHEEL:
		break;
	}
	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	invalidate();
}

void MyGLCanvas::loadPLY(std::string filename) {
	delete myObjectPLY;
	myObjectPLY = new ply(filename);
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);
}

void MyGLCanvas::loadEnvironmentTexture(std::string filename) {
	myTextureManager->deleteTexture("environMap");
	myTextureManager->loadTexture("environMap", filename);
}

void MyGLCanvas::loadObjectTexture(std::string filename) {
	myTextureManager->deleteTexture("objectTexture");
	myTextureManager->loadTexture("objectTexture", filename);
}