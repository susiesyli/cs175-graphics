#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <random> 

#include "TextureManager.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"

class MyGLCanvas : public Fl_Gl_Window {
public:
	glm::vec3 eyePosition;
	glm::vec3 rotVec;
	glm::vec3 lookatPoint;
	glm::vec3 lightPos;
	glm::vec3 rotWorldVec;

	float lightAngle;
	float lightIntensity;
	int viewAngle;
	float clipNear;
	float clipFar;
	float scaleFactor;
	float textureBlend;
	int repeatU;
	int repeatV;

	// waves
	glm::vec2 waveSpeed;
	float waveAmplitude;
	float waveFrequency;
	float waveSpeedX;
	float waveSpeedY;

	// fog
	glm::vec3 fogColor;
	float fogDensity;
	float noiseScale;
	float noiseSpeed;
	bool useFog;

	// rain
	int numDrops;
    int numRainDrops;
	float rainSpeed;
	bool useRain;


	MyGLCanvas(int x, int y, int w, int h, const char* l = 0);
	~MyGLCanvas();

	void loadPLY(std::string filename);
	void loadEnvironmentTexture(std::string filename);
	void loadObjectTexture(std::string filename);
	void reloadShaders();

private:
	void draw();
	void drawScene();

	void initShaders();
	void initDrops();
    void initRain();
    void initRipples();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	TextureManager* myTextureManager;
	ShaderManager* myShaderManager;
	ply* myObjectPLY;
	ply* myEnvironmentPLY;
	ply* mySunPLY;
	ply* myRainPLY;
	ply* mySkyboxPLY;
	ply* myStarPLY;
	ply* myMoonPLY;

	glm::mat4 perspectiveMatrix;

	bool firstTime;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

	std::vector<glm::vec3> rainDrops;
	std::vector<glm::vec4> actualRain;
    std::vector<glm::vec2> ripples;
};

#endif // !MYGLCANVAS_H