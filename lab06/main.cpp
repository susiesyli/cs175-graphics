/*  =================== File Information =================
	File Name: main.cpp
	Description:
	Author: Michael Shah

	Purpose: Driver for 3D program to load .ply models
	Usage:
	===================================================== */

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>

#include "MyGLCanvas.h"

using namespace std;

class MyAppWindow;
MyAppWindow *win;

class MyAppWindow : public Fl_Window {
public:
	Fl_Slider* rotXSlider;
	Fl_Slider* rotYSlider;
	Fl_Slider* rotZSlider;
	Fl_Slider* scaleSlider;
	Fl_Button* reloadButton;
	Fl_Button* animateLightButton;

    // added for color control 
    Fl_Slider* redSlider;
    Fl_Slider* greenSlider;
    Fl_Slider* blueSlider;

	MyGLCanvas* canvas;

public:
	// APP WINDOW CONSTRUCTOR
	MyAppWindow(int W, int H, const char*L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}

private:
	// Someone changed one of the sliders
	static void rotateCB(Fl_Widget* w, void* userdata) {
		float value = ((Fl_Slider*)w)->value();
		*((float*)userdata) = value;
	}

	static void toggleCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Button*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
	}

	static void reloadCB(Fl_Widget* w, void* userdata) {
		win->canvas->reloadShaders();
	}
};


MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	begin();
	// OpenGL window

	canvas = new MyGLCanvas(10, 10, w() - 110, h() - 20);

	Fl_Pack* pack = new Fl_Pack(w() - 100, 30, 100, h(), "Control Panel");
	pack->box(FL_DOWN_FRAME);
	pack->labelfont(1);
	pack->type(Fl_Pack::VERTICAL);
	pack->spacing(0);
	pack->begin();

	//slider for controlling rotation
	Fl_Box *rotXTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateX");
	rotXSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotXSlider->align(FL_ALIGN_TOP);
	rotXSlider->type(FL_HOR_SLIDER);
	rotXSlider->bounds(-359, 359);
	rotXSlider->step(1);
	rotXSlider->value(canvas->rotVec.x);
	rotXSlider->callback(rotateCB, (void*)(&(canvas->rotVec.x)));

	Fl_Box *rotYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateY");
	rotYSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotYSlider->align(FL_ALIGN_TOP);
	rotYSlider->type(FL_HOR_SLIDER);
	rotYSlider->bounds(-359, 359);
	rotYSlider->step(1);
	rotYSlider->value(canvas->rotVec.y);
	rotYSlider->callback(rotateCB, (void*)(&(canvas->rotVec.y)));

	Fl_Box *rotZTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateZ");
	rotZSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotZSlider->align(FL_ALIGN_TOP);
	rotZSlider->type(FL_HOR_SLIDER);
	rotZSlider->bounds(-359, 359);
	rotZSlider->step(1);
	rotZSlider->value(canvas->rotVec.z);
	rotZSlider->callback(rotateCB, (void*)(&(canvas->rotVec.z)));

	Fl_Box *scaleTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale");
	scaleSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	scaleSlider->align(FL_ALIGN_TOP);
	scaleSlider->type(FL_HOR_SLIDER);
	scaleSlider->bounds(1, 5);
	scaleSlider->step(0.1);
	scaleSlider->value(canvas->scaleFactor);
	scaleSlider->callback(rotateCB, (void*)(&(canvas->scaleFactor)));
	pack->end();


	Fl_Pack* packShaders = new Fl_Pack(w() - 100, 230, 100, h(), "Shaders");
	packShaders->box(FL_DOWN_FRAME);
	packShaders->labelfont(1);
	packShaders->type(Fl_Pack::VERTICAL);
	packShaders->spacing(0);
	packShaders->begin();

	animateLightButton = new Fl_Check_Button(0, 100, pack->w() - 20, 20, "Animate Light");
	animateLightButton->callback(toggleCB, (void*)(&(canvas->animateLight)));
	animateLightButton->value(canvas->animateLight);


	reloadButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Reload");
	reloadButton->callback(reloadCB, (void*)this);

	packShaders->end();




    // color control 
     // Add a new pack for light color controls
    Fl_Pack* packColor = new Fl_Pack(w() - 100, 330, 100, h(), "Light Color");
    packColor->box(FL_DOWN_FRAME);
    packColor->labelfont(1);
    packColor->type(Fl_Pack::VERTICAL);
    packColor->spacing(0);
    packColor->begin();

    // Red slider
    Fl_Box *redTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Red");
    redSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    redSlider->align(FL_ALIGN_TOP);
    redSlider->type(FL_HOR_SLIDER);
    redSlider->bounds(0, 1.0);
    redSlider->step(0.1);
    redSlider->value(canvas->lightColor.x);
    redSlider->callback(rotateCB, (void*)(&(canvas->lightColor.x)));

    // Green slider
    Fl_Box *greenTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Green");
    greenSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    greenSlider->align(FL_ALIGN_TOP);
    greenSlider->type(FL_HOR_SLIDER);
    greenSlider->bounds(0, 1.0);
    greenSlider->step(0.1);
    greenSlider->value(canvas->lightColor.y);
    greenSlider->callback(rotateCB, (void*)(&(canvas->lightColor.y)));

    // Blue slider
    Fl_Box *blueTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Blue");
    blueSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    blueSlider->align(FL_ALIGN_TOP);
    blueSlider->type(FL_HOR_SLIDER);
    blueSlider->bounds(0, 1.0);
    blueSlider->step(0.1);
    blueSlider->value(canvas->lightColor.z);
    blueSlider->callback(rotateCB, (void*)(&(canvas->lightColor.z)));

    packColor->end();



	end();
}


/**************************************** main() ********************/
int main(int argc, char **argv) {
	win = new MyAppWindow(600, 500, "Normal Mapping");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}