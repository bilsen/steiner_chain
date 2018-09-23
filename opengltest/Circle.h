#include <math.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <complex>
#include <iostream>
#include "glCircles.h"



using namespace std;

class Circle {

public:
	glm::vec4 color;
	float r;
	complex<float> pos;
	
	Circle(complex<float> position, float radius) {
		r = radius;
		pos = position;
		color = glm::vec4(1, 1, 1, 1);
	}
	Circle() : Circle(complex<float>(0., 0.), 1.) {};

	void setRandomColor(int level) {
		color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	void scaleFromPoint(complex<float> from, float scale) {
		pos = (pos - from) * scale + from;
		r *= scale;
	}
	void printInfo() {
		cout << "Center: ";
		cout << pos;
		cout << ". Radius: ";
		cout << r << endl;
	}


	void draw(glCircles * drawHandle, Shader * shaderHandle, bool fill) {
		color.a = min(abs(float(1.0 - 0.9f / r)), 1.0f);

		if (fill)
			drawHandle->drawFilled(glm::vec3(pos.real(), pos.imag(), 0), r, shaderHandle, color = color);
		else
			drawHandle->drawBorder(glm::vec3(pos.real(), pos.imag(), 0), r, shaderHandle, color = color);

	}
};
