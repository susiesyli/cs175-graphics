#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;
	
	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 10.0f;
	scaleFactor = 1.0f;
	animateLight = 1.0;

	firstTime = true;

	myObject = new SceneObject(175);
	myShaderManager = new ShaderManager();
	myPLY = new ply("./data/bunny.ply");
}

MyGLCanvas::~MyGLCanvas() {
	delete myObject;
	delete myShaderManager;
	delete myPLY;
}


void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
	//setting up camera info
	glm::mat4 modelViewMatrix = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myModelviewMatrix"), 1, false, glm::value_ptr(modelViewMatrix));

	if (animateLight == 1) {
		lightPos.x += 0.05f;
		lightPos.y = 0.5f;
		lightPos.z += 0.05f;
	}

	glm::vec3 rotateLightPos = glm::vec3(sin(lightPos.x), lightPos.y, cos(lightPos.z));
	//TODO: pass lighting information to the shaders


	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	myPLY->renderVBO();
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glm::mat4 perspectiveMatrix;
	perspectiveMatrix = glm::perspective(TO_RADIANS(viewAngle), xy_aspect, clipNear, clipFar);
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myProjectionMatrix"), 1, false, glm::value_ptr(perspectiveMatrix));
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err)	{
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

void MyGLCanvas::initShaders() {
	myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

	myPLY->buildArrays(); 
	myPLY->bindVBO(myShaderManager->program);
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

	myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

	myPLY->bindVBO(myShaderManager->program);
	invalidate();
}
