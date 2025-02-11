#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape {
public:
	Cube() {};
	~Cube() {};

	OBJ_TYPE getType() {
		return SHAPE_CUBE;
	}

	void draw() {
	};


	void drawNormal() {
	};

private:
};

#endif