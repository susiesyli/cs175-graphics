#include <FL/gl.h>
#include <FL/glut.h>
#include "SolarSystem.h"

#define SEGMENTS 20

SolarSystem::SolarSystem(){

}

SolarSystem::~SolarSystem(){

}

// Render this with push and pop operations
// or alternatively implement helper functions
// for each of the planets with parameters.
void SolarSystem::render(){
	
	// Some ideas for constants that can be used for
	// rotating the planets.
	static float rotation = 0.1;
	static float rotation2 = 0.1;
	static float rotation2moon = 0.1;
	static float rotation3 = 0.1;
	static float orbitSpeed = 0.15;
	static float moonOrbitX = 0.1;
	static float moonOrbitY = 0.1;
	static float moonOrbitSpeed = 1;

	glPushMatrix();

		// The Sun
		glPushMatrix();
			glRotatef(rotation,0,1,0);
			glColor3f(0.96f,0.85f,0.26f);
			glutSolidSphere(0.12, SEGMENTS, SEGMENTS);
		glPopMatrix();


	// Add more planets, moons, and rings here!	
    // planet 1
		glPushMatrix();
			glRotatef(rotation,0,1,0);
            glTranslatef(0.9, 0, 0);
			glColor3f(0.0f,0.85f,0.26f);
			glutSolidSphere(0.05, SEGMENTS, SEGMENTS);
		glPopMatrix();

		glPushMatrix();
			glRotatef(rotation2,0,0,1);
            glTranslatef(0.7, 0, 0);
            glPushMatrix();
                glRotatef(rotation2moon,0,0,1);
                glTranslatef(0.2, 0, 0);
                glPushMatrix();
                    glRotatef(rotation2moon,0.5,0.5,0);
                    glTranslatef(0.05, 0, 0);;
                    glColor4f(1.0f,0.5f,0.0f,0.0f);
                    glutSolidSphere(0.02, SEG