	#ifndef SHAPE_H
#define SHAPE_H

#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define PI glm::pi<float>()  //PI is now a constant for 3.14159....

enum OBJ_TYPE {
	SHAPE_CUBE = 0,
	SHAPE_CYLINDER = 1,
	SHAPE_CONE = 2,
	SHAPE_SPHERE = 3,
	SHAPE_SPECIAL1 = 4,
	SHAPE_SPECIAL2 = 5,
	SHAPE_SPECIAL3 = 6,
	SHAPE_MESH = 7
};

class Shape {
public:
	static int m_segmentsX;
	static int m_segmentsY;

	Shape() {
		setSegments(10, 10);
	};
	~Shape() {};

	void setSegments(int segX, int segY) {
		m_segmentsX = segX;
		m_segmentsY = segY;
	}

	virtual OBJ_TYPE getType() = 0;
	virtual void draw() {};
	virtual float draw(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix) { return 0.0f;  };
	//virtual glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 ray, glm::mat4 transformMatrix, double t) { return glm::vec3(0.0f, 0.0f, 0.0f); };
	virtual glm::vec3 drawNormal(glm::vec3 eyePoint, glm::vec3 worldSpacePos, glm::mat4 transformMatrix, double t) { return glm::vec3(0.0f, 0.0f, 0.0f); };
	virtual glm::vec2 getUVCoordinates(glm::vec3 ray) { return glm::vec2(1.0f, 1.0f); }

protected:
	void normalizeNormal (float x, float y, float z) {
		normalizeNormal (glm::vec3(x, y, z));
	};

	void normalizeNormal (glm::vec3 v) {
		glm::vec3 tmpV = glm::normalize(v);
		glNormal3f(tmpV.x, tmpV.y, tmpV.z);
	};
};

#endif