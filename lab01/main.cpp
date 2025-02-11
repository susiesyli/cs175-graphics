/*  =================== File Information =================
    File Name: main.cpp
    Description:
    Author: Michael Shah

    Purpose: Driver for 3D program to load .ply models
    =====================================================
*/

#define GL_SILENCE_DEPRECATION

#include "ply.h"
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Window.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/glut.H>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>

class MyGLCanvas : public Fl_Gl_Window {
public:
    int   wireframe, filled;
    int   rotY;
    float scale;
    /****************************************/
    /*         PLY Object                   */
    /****************************************/
    ply *myPLY = NULL;

    MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
    ~MyGLCanvas();

private:
    void draw();
    int  handle(int);
    void resize(int x, int y, int w, int h);
};

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) :
    Fl_Gl_Window(x, y, w, h, l) {
    mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
    wireframe = 0;
    filled    = 1;
    rotY      = 0;
    scale     = 1.0f;
    myPLY     = new ply();
}

MyGLCanvas::~MyGLCanvas() {
    delete myPLY;
}

void MyGLCanvas::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!valid()) { // this is called when the GL canvas is set up for the first
                    // time...
        puts("establishing GL context");

        float xy_aspect;
        xy_aspect = (float)w() / (float)h();
        //
        glViewport(0, 0, w(), h());
        // Determine if we are modifying the camera(GL_PROJECITON) matrix(which
        // is our viewing volume) Otherwise we could modify the object
        // transormations in our world with GL_MODELVIEW
        glMatrixMode(GL_PROJECTION);
        // Reset the Projection matrix to an identity matrix
        glLoadIdentity();
        // The frustrum defines the perspective matrix and produces a
        // perspective projection. It works by multiplying the current matrix(in
        // this case, our matrix is the GL_PROJECTION) and multiplies it.
        glFrustum(-xy_aspect * .125, xy_aspect * .125, -.125, .125, .1, 15.0);

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glShadeModel(GL_FLAT);

        GLfloat light_pos0[] = {0.0f, 0.0f, 1.0f, 0.0f};
        GLfloat diffuse[]    = {0.5f, 0.5f, 0.5f, 0.0f};
        GLfloat ambient[]    = {0.7f, 0.7f, 0.7f, 1.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        /**********************************************/
        /*    Enable normalizing normal vectors       */
        /*    (e.g. normals not affected by glScalef) */
        /**********************************************/

        glEnable(GL_NORMALIZE);

        /****************************************/
        /*          Enable z-buferring          */
        /****************************************/

        glEnable(GL_DEPTH_TEST);
        glPolygonOffset(1, 1);
    }

    // Clear the buffer of colors in each bit plane.
    // bit plane - A set of bits that are on or off (Think of a black and white
    // image)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the mode so we are modifying our objects.
    glMatrixMode(GL_MODELVIEW);
    // Load the identify matrix which gives us a base for our object
    // transformations (i.e. this is the default state)
    glLoadIdentity();

    // here we are moving the camera back by 0.75 on the z-axis
    glTranslatef(0, 0, -0.75);

    // allow for user controlled rotation and scaling
    glScalef(scale, scale, scale);
    glRotatef(rotY, 0.0, 1.0, 0.0);

    // draw the axes
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(1.0, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0.0, 1.0, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1.0);
    glEnd();

    if (filled) {
        glEnable(GL_LIGHTING);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glColor3f(0.6, 0.6, 0.6);
        glPolygonMode(GL_FRONT, GL_FILL);
        myPLY->render();
    }

    if (wireframe) {
        glDisable(GL_LIGHTING);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glColor3f(1.0, 1.0, 0.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        myPLY->render();
    }
    // no need to call swap_buffer as it is automatically called
}

int MyGLCanvas::handle(int e) {
    switch (e) {
    case FL_ENTER: cursor(FL_CURSOR_CROSS); break;
    case FL_LEAVE: cursor(FL_CURSOR_DEFAULT); break;
    }
    return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
    Fl_Gl_Window::resize(x, y, w, h);
    puts("resize called");
}


class MyAppWindow : public Fl_Window {
public:
    Fl_Slider  *rotYSlider;
    Fl_Slider  *scaleSlider;
    Fl_Button  *wireButton;
    Fl_Button  *fillButton;
    Fl_Button  *openFileButton;
    MyGLCanvas *canvas;

public:
    // APP WINDOW CONSTRUCTOR
    MyAppWindow(int W, int H, const char *L = 0) : Fl_Window(W, H, L) {
        begin();
        // OpenGL window

        canvas = new MyGLCanvas(10, 10, w() - 110, h() - 20);

        Fl_Pack *pack = new Fl_Pack(w() - 100, 30, 100, h(), "Control Panel");
        pack->box(FL_DOWN_FRAME);
        pack->labelfont(1);
        pack->type(Fl_Pack::VERTICAL);
        pack->spacing(0);
        pack->begin();

        openFileButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Load File");
        openFileButton->callback(loadFileCB, (void *)this);

        wireButton =
            new Fl_Check_Button(0, 100, pack->w() - 20, 20, "Wireframe");
        wireButton->callback(wireCB, (void *)this);
        wireButton->value(canvas->wireframe);

        fillButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Fill");
        fillButton->callback(fillCB, (void *)this);
        fillButton->value(canvas->filled);

        // slider for controlling rotation
        Fl_Box *rotTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Rotate");
        rotYSlider         = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
        rotYSlider->align(FL_ALIGN_TOP);
        rotYSlider->type(FL_HOR_SLIDER);
        rotYSlider->bounds(-359, 359);
        rotYSlider->step(1);
        rotYSlider->value(canvas->rotY);
        rotYSlider->callback(rotYCB, (void *)this);

        // slider for controlling scaling
        Fl_Box *scaleTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale");
        scaleSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
        scaleSlider->align(FL_ALIGN_TOP);
        scaleSlider->type(FL_HOR_SLIDER);
        scaleSlider->bounds(0.2, 3.0);
        scaleSlider->value(canvas->scale);
        scaleSlider->callback(scaleCB, (void *)this);

        pack->end();

        end();

        resizable(this); // make window resizable

        Fl::add_idle((Fl_Idle_Handler)(void *)idleCB, (void *)this);
    }

    static void idleCB(void *data) {
        MyAppWindow *win = (MyAppWindow *)data;
        win->canvas->redraw();
    }

private:
    // Slider callback functions.
    static void rotYCB(Fl_Widget *w, void *data) {
        MyAppWindow *win  = (MyAppWindow *)data;
        win->canvas->rotY = ((Fl_Slider *)w)->value();
    }

    static void scaleCB(Fl_Widget *w, void *data) {
        MyAppWindow *win   = (MyAppWindow *)data;
        win->canvas->scale = ((Fl_Slider *)w)->value();
    }

    static void wireCB(Fl_Widget *w, void *data) {
        MyAppWindow *win       = (MyAppWindow *)data;
        win->canvas->wireframe = ((Fl_Button *)w)->value();
    }

    static void fillCB(Fl_Widget *w, void *data) {
        MyAppWindow *win    = (MyAppWindow *)data;
        win->canvas->filled = ((Fl_Button *)w)->value();
    }

    static void loadFileCB(Fl_Widget *w, void *data) {
        MyAppWindow *win = (MyAppWindow *)data;

        Fl_File_Chooser G_chooser("", "", Fl_File_Chooser::MULTI, "");
        G_chooser.show();
        // Block until user picks something.
        //     (The other way to do this is to use a callback())
        //
        G_chooser.directory("./data");
        while (G_chooser.shown()) {
            Fl::wait();
        }

        // Print the results
        if (G_chooser.value() == NULL) {
            printf("User cancelled file chooser\n");
            return;
        }

        std::cout << "Loading new ply file from: " << G_chooser.value()
                  << std::endl;
        // Reload our model
        win->canvas->myPLY->reload(G_chooser.value());
        // Print out the attributes
        win->canvas->myPLY->printAttributes();

        win->canvas->redraw();
    }
};

/************* main() ********************/
int main(int argc, char **argv) {
    MyAppWindow win(600, 500, "Shape Files");
    win.show();
    return (Fl::run());
}