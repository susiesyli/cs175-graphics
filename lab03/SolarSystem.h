#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include <math.h>

#define PI 3.141592653589793238462643383279502


class SolarSystem{
	public:
		SolarSystem();
		~SolarSystem();

		void render();
		void draw_orbits();

private:
	void draw_circle(float radius);
};

#endif