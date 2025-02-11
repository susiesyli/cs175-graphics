#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

using namespace std;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char* l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	rotWorldVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;

	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 20000.0f;
	lightAngle = 0.0f;
	lightIntensity = 0.9f;
	textureBlend = 1.0f;
	repeatU = 5;
	repeatV = 5;

	waveSpeed = glm::vec2(0.1f, 0.05f);
	waveAmplitude = 0.02f;
	waveFrequency = 1.5f;

	fogColor = glm::vec3(0.7f, 0.7f, 0.7f);
	fogDensity = 0.2f;

	noiseScale = 0.1f;
	noiseSpeed = 0.5f;

	numDrops = 2000;
    numRainDrops = 10000;
	useFog = false;
	useRain = false;

	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();

	// read in geometry models 
	myObjectPLY = new ply("./data/cube.ply");
	myEnvironmentPLY = new ply("./data/sphere.ply");
	mySunPLY = new ply("./data/sphere.ply");
	myRainPLY = new ply("./data/sphere.ply");
	myStarPLY = new ply("./data/sphere.ply");
	myMoonPLY = new ply("./data/sphere.ply");

	// create rain and stars
	initDrops();
    initRain();
    initRipples();
}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete myObjectPLY;
	delete myEnvironmentPLY;
	delete mySunPLY;
	delete myRainPLY;
    delete myStarPLY;
}

void MyGLCanvas::initShaders() {
	myTextureManager->loadTexture("environMap", "./data/new_skyyy-2.ppm");
	myTextureManager->loadTexture("objectTexture", "./data/waveey.ppm");
	myTextureManager->loadTexture("moonTexture", "./data/moon.ppm");

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->buildArrays();
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->buildArrays();
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	myShaderManager->addShaderProgram("rainShaders", "shaders/330/rain-vert.shader", "shaders/330/rain-frag.shader");
	myRainPLY->buildArrays();
	myRainPLY->bindVBO(myShaderManager->getShaderProgram("rainShaders")->programID);

	myShaderManager->addShaderProgram("starShaders", "shaders/330/stars-vert.shader", "shaders/330/stars-frag.shader");
	myStarPLY->buildArrays();
	myStarPLY->bindVBO(myShaderManager->getShaderProgram("starShaders")->programID);
	myShaderManager->addShaderProgram("moonShaders", "shaders/330/moon-vert.shader", "shaders/330/moon-frag.shader");
	myMoonPLY->buildArrays();
	myMoonPLY->bindVBO(myShaderManager->getShaderProgram("moonShaders")->programID);
}

void MyGLCanvas::initDrops() {
	int grid_size = static_cast<int>(std::sqrt(numDrops));
	float step_size_x = 10.0f / (grid_size - 1);
	float step_size_z = 10.0f / (grid_size - 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> random_offset(-step_size_x / 2.0f, step_size_x / 2.0f);

	// Generate the coordinates uniformly across the grid
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
            float base_x = -5 + i * step_size_x;
            float base_z = -5 + j * step_size_z;

            // Add random offset to base positions
            float x = base_x + random_offset(gen) * 2;
            float z = base_z + random_offset(gen) * 2;
            float y = 0.75 + random_offset(gen) * 10;
			glm::vec3 dropLocation = glm::vec3(x, y, z);
			rainDrops.emplace_back(dropLocation);
		}
	}
}

void MyGLCanvas::initRain() {
	int grid_size = static_cast<int>(std::sqrt(numRainDrops)); 
	float step_size_x = 10.0f / (grid_size - 1);
	float step_size_z = 10.0f / (grid_size - 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> random_offset(-step_size_x / 2.0f, step_size_x / 2.0f);

	// Generate the coordinates uniformly across the grid
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
            float base_x = -5 + i * step_size_x;
            float base_z = -5 + j * step_size_z;

            std::random_device rdAngle;
            std::mt19937 genAngle(rdAngle());
            std::uniform_real_distribution<float> random_Angle(TO_RADIANS(-2.5f), TO_RADIANS(2.5f));

            // Add random offset to base positions
            float x = base_x + random_offset(gen) * 2;
            float z = base_z + random_offset(gen) * 2;
            float y = 1.0 + random_offset(gen) * 100;
			// add random angle
            float w = random_Angle(genAngle);
			glm::vec4 dropLocation = glm::vec4(x, y, z, w);
			actualRain.emplace_back(dropLocation);
		}
	}
}

void MyGLCanvas::initRipples() {
    int grid_size = static_cast<int>(std::sqrt(numRainDrops)); 
	float step_size_x = 10.0f / (grid_size - 1);
	float step_size_z = 10.0f / (grid_size - 1);

    for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
            float base_x = -5 + i * step_size_x;
            float base_z = -5 + j * step_size_z;

            ripples.emplace_back(glm::vec2(base_x, base_z));
		}
	}
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
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.1f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 0.1f, 10.0f));

	glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	//Pass first texture info to our shader 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("environMap"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("objectTexture"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("moonTexture"));
	// Draw Ocean
	glUseProgram(myShaderManager->getShaderProgram("objectShaders")->programID);

	GLuint objectShaderProgram = myShaderManager->getShaderProgram("objectShaders")->programID;

	auto currentTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> elapsedTime = currentTime - startTime;
	float totalTime = elapsedTime.count();
    std::chrono::duration<float> deltaTime = currentTime - lastTime;
    float delta = deltaTime.count();
    lastTime = currentTime;

	GLint modelLoc = glGetUniformLocation(objectShaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(objectShaderProgram, "view");
	GLint projLoc = glGetUniformLocation(objectShaderProgram, "projection");
	GLint lightPosLoc = glGetUniformLocation(objectShaderProgram, "lightPos");
	GLint lightIntensityLoc = glGetUniformLocation(objectShaderProgram, "lightIntensity");
	GLint viewPosLoc = glGetUniformLocation(objectShaderProgram, "viewPos");
	GLint textureBlendLoc = glGetUniformLocation(objectShaderProgram, "textureBlend");
	GLint repeatULoc = glGetUniformLocation(objectShaderProgram, "repeatU");
	GLint repeatVLoc = glGetUniformLocation(objectShaderProgram, "repeatV");
	GLint timeLoc = glGetUniformLocation(objectShaderProgram, "time");
	GLint waveSpeedLoc = glGetUniformLocation(objectShaderProgram, "waveSpeed");
	GLint waveAmplitudeLoc = glGetUniformLocation(objectShaderProgram, "waveAmplitude");
	GLint waveFrequencyLoc = glGetUniformLocation(objectShaderProgram, "waveFrequency");
	GLint fogColorLoc = glGetUniformLocation(objectShaderProgram, "fogColor");
	GLint fogDensityLoc = glGetUniformLocation(objectShaderProgram, "fogDensity");
	GLint noiseScaleLoc = glGetUniformLocation(objectShaderProgram, "noiseScale");
	GLint noiseSpeedLoc = glGetUniformLocation(objectShaderProgram, "noiseSpeed");
	GLint useFogLoc = glGetUniformLocation(objectShaderProgram, "useFog");
	GLint moonVisibleLoc = glGetUniformLocation(objectShaderProgram, "moonVisible");
    
	// Pass matrix uniforms 
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

	// add pass light angle 
	glm::vec4 lightPos(0.0f, 0.0f, 1.0f, 0.0f);
	// add light rotation angle 
	lightPos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIANS(lightAngle), glm::vec3(0.0, 0.0, 1.0));
	lightPos = rotationMatrix * lightPos;

	if (lightAngle < -90 || lightAngle > 90) {
		lightPos = -lightPos;
	}
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	
	bool moonVisible = false;
	if (lightAngle < -90 || lightAngle > 90) {
		lightPos = -lightPos;
		moonVisible = true;
	}

	glm::vec4 rotatedEye = glm::inverse(viewMatrix) * glm::vec4(eyePosition, 1.0f);
	glm::vec3 transformedEye = glm::vec3(rotatedEye);

	glUniform3fv(viewPosLoc, 1, glm::value_ptr(transformedEye));
	glUniform1f(textureBlendLoc, textureBlend);
	glUniform1f(repeatULoc, repeatU);
	glUniform1f(repeatVLoc, repeatV);
	glUniform1f(timeLoc, totalTime);
	glUniform2f(waveSpeedLoc, waveSpeed[0], waveSpeed[1]);
	glUniform1f(waveAmplitudeLoc, waveAmplitude);
	glUniform1f(waveFrequencyLoc, waveFrequency);
	glUniform1f(lightIntensityLoc, lightIntensity);
	glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogColor));
	glUniform1f(fogDensityLoc, fogDensity);
	glUniform1f(noiseScaleLoc, noiseScale);
	glUniform1f(noiseSpeedLoc, noiseSpeed);
	glUniform1i(useFogLoc, useFog);
	glUniform1i(moonVisibleLoc, moonVisible);

	GLint rippleLoc = glGetUniformLocation(objectShaderProgram, "drops");
    glUniform1fv(rippleLoc, ripples.size(), glm::value_ptr(ripples.front()));

	// Pass texture units
	GLint environMapLoc = glGetUniformLocation(objectShaderProgram, "environMap");
	GLint objectTextureLoc = glGetUniformLocation(objectShaderProgram, "objectTexture");
	glUniform1i(environMapLoc, 0);  // GL_TEXTURE0
	glUniform1i(objectTextureLoc, 1);  // GL_TEXTURE1
	myObjectPLY->renderVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	// 2. draw sky
	glUseProgram(myShaderManager->getShaderProgram("environmentShaders")->programID);
	// Get shader program
	GLuint environmentShaderProgram = myShaderManager->getShaderProgram("environmentShaders")->programID;
	// Variable binding for environment shader
	GLint envModelLoc = glGetUniformLocation(environmentShaderProgram, "model");
	GLint envViewLoc = glGetUniformLocation(environmentShaderProgram, "view");
	GLint envProjLoc = glGetUniformLocation(environmentShaderProgram, "projection");

	GLint skyLightPosLoc = glGetUniformLocation(environmentShaderProgram, "lightPos");
	glUniform3fv(skyLightPosLoc, 1, glm::value_ptr(lightPos));
	GLint skyLightIntensityLoc = glGetUniformLocation(environmentShaderProgram, "lightIntensity");
	glUniform1f(skyLightIntensityLoc, lightIntensity);

	// Create environment model matrix (scaled up)
	glm::mat4 environmentModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(envModelLoc, 1, GL_FALSE, glm::value_ptr(environmentModelMatrix));
	glUniformMatrix4fv(envViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(envProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	// Pass texture unit for environment shader
	GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "environMap");
	glUniform1i(environMapLocEnv, 0);  // GL_TEXTURE0

	myEnvironmentPLY->renderVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	// draw sun sphere
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(myShaderManager->getShaderProgram("sunShaders")->programID);
	// Get shader program
	GLuint sunShaderProgram = myShaderManager->getShaderProgram("sunShaders")->programID;
	// Variable binding for environment shader
	GLint sunModelLoc = glGetUniformLocation(sunShaderProgram, "sunModel");
	GLint sunViewLoc = glGetUniformLocation(sunShaderProgram, "sunView");
	GLint sunProjLoc = glGetUniformLocation(sunShaderProgram, "sunProjection");
	GLint sunLightIntensityLoc = glGetUniformLocation(sunShaderProgram, "lightIntensity");
	glUniform1f(sunLightIntensityLoc, lightIntensity);
	// Create sun model matrix (scaled up) 
	glm::mat4 sunModelMatrix = glm::mat4(1.0f);
	sunModelMatrix = glm::translate(sunModelMatrix, glm::vec3(lightPos));
	sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(sunModelLoc, 1, GL_FALSE, glm::value_ptr(sunModelMatrix));
	glUniformMatrix4fv(sunViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(sunProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	mySunPLY->renderVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw star spheres
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get shader program
	glUseProgram(myShaderManager->getShaderProgram("starShaders")->programID);
	GLuint starShaderProgram = myShaderManager->getShaderProgram("starShaders")->programID;
    // Variable binding for environment shader
	GLint starModelLoc = glGetUniformLocation(starShaderProgram, "starModel");
	GLint starViewLoc = glGetUniformLocation(starShaderProgram, "starView");
	GLint starProjLoc = glGetUniformLocation(starShaderProgram, "starProjection");
	GLint starLightIntensityLoc = glGetUniformLocation(starShaderProgram, "lightIntensity");
    GLint starLightPosLoc = glGetUniformLocation(starShaderProgram, "lightPos");
	glUniform3fv(starLightPosLoc, 1, glm::value_ptr(lightPos));

	glUniform1f(starLightIntensityLoc, lightIntensity);

    for (int i = 0; i < numDrops; i++) {
       // Create sun model matrix (scaled up) 
       glm::mat4 starModelMatrix = glm::mat4(1.0f);
       glm::vec3 thisStar = rainDrops[i];
       starModelMatrix = glm::translate(starModelMatrix, thisStar);

       starModelMatrix = glm::scale(starModelMatrix, glm::vec3(0.0025f, 0.0025f, 0.0025f));
       // Pass matrix uniforms for environment shader
       glUniformMatrix4fv(starModelLoc, 1, GL_FALSE, glm::value_ptr(starModelMatrix));
       glUniformMatrix4fv(starViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
       glUniformMatrix4fv(starProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
       myStarPLY->renderVBO(myShaderManager->getShaderProgram("starShaders")->programID);
    }

    // draw rain spheres
	// Get shader program
	if (useRain) {
		glUseProgram(myShaderManager->getShaderProgram("rainShaders")->programID);
		GLuint rainShaderProgram = myShaderManager->getShaderProgram("rainShaders")->programID;
		// Variable binding for environment shader
		GLint rainModelLoc = glGetUniformLocation(rainShaderProgram, "rainModel");
		GLint rainViewLoc = glGetUniformLocation(rainShaderProgram, "rainView");
		GLint rainProjLoc = glGetUniformLocation(rainShaderProgram, "rainProjection");
		GLint rainLightIntensityLoc = glGetUniformLocation(rainShaderProgram, "lightIntensity");
		GLint rainLightPosLoc = glGetUniformLocation(rainShaderProgram, "lightPos");
		GLint deltaTimeLoc = glGetUniformLocation(rainShaderProgram, "deltaTime");
		GLint fallSpeedLoc = glGetUniformLocation(rainShaderProgram, "fallSpeed");
		glUniform3fv(rainLightPosLoc, 1, glm::value_ptr(lightPos));

		glUniform1f(rainLightIntensityLoc, lightIntensity);
		glUniform1f(deltaTimeLoc, delta);

		for (int i = 0; i < numRainDrops; i++) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> random_offset(0, 1);

			float fallSpeed = 2.0f + random_offset(gen) * 2;
			float y = fallSpeed * delta;
			actualRain[i].y -= y;  // Move down by fallSpeed * deltaTime
			actualRain[i].x += y * (tan(actualRain[i].w) + tan(TO_RADIANS(noiseScale * 45)));

			// Reset position if raindrop hits water level
			float waterLevel = -0.1f;
			if (actualRain[i].y <= waterLevel) {
				actualRain[i].y = 5.0f;  // Reset to starting height
				actualRain[i].x = static_cast<float>(rand() % 200 - 100) / 10.0f;  // Random x position
				actualRain[i].z = static_cast<float>(rand() % 200 - 100) / 10.0f;  // Random z position
			}

			// Create sun model matrix (scaled up) 
			glm::mat4 rainModelMatrix = glm::mat4(1.0f);
			glm::vec3 thisRain = actualRain[i];
			rainModelMatrix = glm::translate(rainModelMatrix, thisRain);
			rainModelMatrix = glm::scale(rainModelMatrix, glm::vec3(0.003f, 0.003f, 0.003f));

			// Pass matrix uniforms for environment shader
			glUniformMatrix4fv(rainModelLoc, 1, GL_FALSE, glm::value_ptr(rainModelMatrix));
			glUniformMatrix4fv(rainViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(rainProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
			// glUniform1f(fallSpeedLoc, 10);
			myRainPLY->renderVBO(myShaderManager->getShaderProgram("rainShaders")->programID);
		}

	}
	
	glUseProgram(myShaderManager->getShaderProgram("moonShaders")->programID);
	GLuint moonShaderProgram = myShaderManager->getShaderProgram("moonShaders")->programID;

	GLint moonModelLoc = glGetUniformLocation(moonShaderProgram, "moonModel");
	GLint moonViewLoc = glGetUniformLocation(moonShaderProgram, "moonView");
	GLint moonProjLoc = glGetUniformLocation(moonShaderProgram, "moonProjection");

	glm::mat4 moonModelMatrix = glm::mat4(1.0f);
	moonModelMatrix = glm::translate(moonModelMatrix, glm::vec3(-lightPos));
	moonModelMatrix = glm::scale(moonModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

	glUniformMatrix4fv(moonModelLoc, 1, GL_FALSE, glm::value_ptr(moonModelMatrix));
	glUniformMatrix4fv(moonViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(moonProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

	GLint moonMapLoc = glGetUniformLocation(moonShaderProgram, "moonMap");
	glUniform1i(moonMapLoc, 3);

	myMoonPLY->renderVBO(myShaderManager->getShaderProgram("moonShaders")->programID);
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
	static int lastX = 0;
	static bool isMouseDown = false;

	switch (e) {
	case FL_DRAG: {
		if (isMouseDown) {
			int dx = Fl::event_x() - lastX; // find magnitute of movement 
			rotWorldVec.y += dx * 0.5f;  // allow horizontal rotation 
			rotWorldVec.y = fmod(rotWorldVec.y, 360.0f); // normalize to 0, 360 range 
			lastX = Fl::event_x(); // Update last mouse position
			redraw();
			return 1;
		}
		break;
	}
	case FL_MOVE:
	case FL_PUSH: {
		if (Fl::event_button() == FL_LEFT_MOUSE) { // pressed mouse or touchpad 
			isMouseDown = true;
			lastX = Fl::event_x();
		}
		return 1;
	}

	case FL_RELEASE: {
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			isMouseDown = false;
		}
		return 1;
	}

	case FL_KEYUP:
	case FL_MOUSEWHEEL: {
		float zoomFactor = Fl::event_dy() * 0.1f;
		eyePosition.z += zoomFactor;
		eyePosition.z = std::max(2.0f, std::min(5.0f, eyePosition.z));

		redraw();
		return 1;
		break;
	}
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

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	myShaderManager->addShaderProgram("rainShaders", "shaders/330/rain-vert.shader", "shaders/330/rain-frag.shader");
	myRainPLY->bindVBO(myShaderManager->getShaderProgram("rainShaders")->programID);

	// myShaderManager->addShaderProgram("cloudShaders", "shaders/330/cloud-vert.shader", "shaders/330/cloud-frag.shader");
	// myCloudPLY->bindVBO(myShaderManager->getShaderProgram("cloudShaders")->programID);

	myShaderManager->addShaderProgram("starShaders", "shaders/330/stars-vert.shader", "shaders/330/stars-frag.shader");
	myStarPLY->bindVBO(myShaderManager->getShaderProgram("starShaders")->programID);
	myShaderManager->addShaderProgram("moonShaders", "shaders/330/moon-vert.shader", "shaders/330/moon-frag.shader");
	myMoonPLY->bindVBO(myShaderManager->getShaderProgram("moonShaders")->programID);

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