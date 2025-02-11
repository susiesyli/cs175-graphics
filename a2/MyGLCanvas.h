#pragma once

#include "scene/SceneData.h"
#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#define GL_SILENCE_DEPRECATION

#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <time.h>
#include <utility>
#include <vector>

#include "objects/Cone.h"
#include "objects/Cube.h"
#include "objects/Cylinder.h"
#include "objects/Shape.h"
#include "objects/Sphere.h"

#include "Camera.h"
#include "scene/SceneParser.h"

class MyGLCanvas : public Fl_Gl_Window {
public:
  glm::vec3 rotVec;
  glm::vec3 eyePosition;

  int wireframe;
  int smooth;
  int fill;
  int normal;
  int segmentsX, segmentsY;

  OBJ_TYPE objType;
  Cube *cube;
  Cylinder *cylinder;
  Cone *cone;
  Sphere *sphere;
  Shape *shape;

  Camera *camera;
  SceneParser *parser;

  MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
  ~MyGLCanvas();
  void renderShape(OBJ_TYPE type);
  void setSegments();
  void loadSceneFile(const char *filenamePath);
  void setShape(OBJ_TYPE type);
  void resetScene();

private:
  vector<pair<ScenePrimitive *, glm::mat4>> flattenedScene;
  void draw();
  void drawScene();
  void drawObject(OBJ_TYPE type);
  void drawAxis();

  int handle(int);
  void resize(int x, int y, int w, int h);
  void updateCamera(int width, int height);
  void setLight(const SceneLightData &light);
  void applyMaterial(const SceneMaterial &material);

  void precomputeSceneTree(SceneNode *root, glm::mat4 transformation);
  glm::mat4 applyTransform(const glm::mat4 &transform,
                           vector<SceneTransformation *> transformVec);
};

#endif // !MYGLCANVAS_H
