#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <glm/glm.hpp>
#include <iostream>
#include <time.h>

#include "Cone.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Shape.h"
#include "Sphere.h"
//#include "Torus.h"


class MyGLCanvas : public Fl_Gl_Window {
public:
    glm::vec3 rotVec;
    glm::vec3 eyePosition;

    int   wireframe;
    int   smooth;
    int   fill;
    int   normal;
    int   segmentsX, segmentsY;
    float scale;

    OBJ_TYPE  objType;
    Cube     *cube;
    Cylinder *cylinder;
    Cone     *cone;
    Sphere   *sphere;
    // Torus* torus;
    Shape *shape;

    MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
    ~MyGLCanvas();
    void setShape(OBJ_TYPE type);
    void setSegments();

private:
    void draw();
    void drawScene();

    void drawAxis();

    int  handle(int);
    void resize(int x, int y, int w, int h);
    void updateCamera(int width, int height);
};

#endif // !MYGLCANVAS_H