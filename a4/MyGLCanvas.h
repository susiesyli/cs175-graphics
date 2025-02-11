#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <iostream>
#include <unordered_map>

#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"
#include "ppm.h"


#include "Camera.h"
#include "scene/SceneParser.h"


struct SceneObject {
	glm::mat4 transformMatrix;
	ScenePrimitive* primitive;  // Use smart pointer
};
struct IntersectionInfo {
	float t;
	SceneObject* object;
	glm::vec3 point;
	glm::vec3 normal;
};

// std::vector<SceneObject> sceneObjects;

class MyGLCanvas : public Fl_Gl_Window {
public:
	glm::vec3 rotVec;
	glm::vec3 eyePosition;
	GLubyte* pixels = NULL;

	int recurseDepth;
	int isectOnly;
	int segmentsX, segmentsY;
	float scale;

	OBJ_TYPE objType;
	Cube* cube;
	Cylinder* cylinder;
	Cone* cone;
	Sphere* sphere;
	Shape* shape;

	Camera* camera;
	SceneParser* parser;

	SceneNode* root;
	SceneGlobalData globalSceneData;
	

	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	float renderShape(OBJ_TYPE type, glm::vec3 ray, glm::mat4 transformMatrix, glm::vec3 eyePoint);
	void setSegments();
	void loadSceneFile(const char* filenamePath);
	void renderScene();

private:
std::unordered_map<std::string, ppm*> textureCache;

	enum color {
		RED,
		BLUE,
		GREEN
	};
	void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);

	glm::vec3 generateRay(int pixelX, int pixelY);
	glm::vec3 getEyePoint();
	glm::vec3 getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t);
    // float calculateObjectLighting(SceneObject object, glm::vec3 objNormal, color color, int numLights, glm::vec3 worldSpacePos, float blend, glm::vec3 textureMap);
	float calculateObjectLighting(SceneObject object, glm::vec3 objNormal, color color, int numLights, glm::vec3 worldSpacePos, float blend, glm::vec3 textureMap, glm::vec3 viewOrigin);

    float intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	void traverseSceneGraph(SceneNode* node, const glm::mat4& parentTransform);
	glm::vec3 traceRay(const glm::vec3& origin, const glm::vec3& ray, int depth, int maxDepth);
	IntersectionInfo findClosestIntersection(const glm::vec3& origin, const glm::vec3& ray);
	bool isInShadow(const glm::vec3& point, const glm::vec3& lightDir, float lightDistance);
	void draw();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	int pixelWidth, pixelHeight;

	bool castRay;

};

#endif // !MYGLCANVAS_H