#include <complex>
#include "Circle.h"


using namespace std;

class Inversion {
public:
	float r;
	std::complex<float> pos;
	Circle circle;

	Circle operator()(Circle x) {
		/*
		Finds the inverted circle by inverting the two points on the circle that lie on the line
		formed by the midpoint of the inversion circle and the midpoint of the circle to be inverted
		*/
		std::complex<float> lineNormalized = (x.pos - pos) / abs(x.pos - pos);

		std::complex<float> point1 = x.pos + lineNormalized * x.r;
		std::complex<float> point2 = x.pos - lineNormalized * x.r;

		std::complex<float> invertedPoint1 = getInvertedPoint(point1);
		std::complex<float> invertedPoint2 = getInvertedPoint(point2);

		Circle result((invertedPoint1 + invertedPoint2) / 2.0f, abs(invertedPoint1 - invertedPoint2) / 2.0f);

		return result;
	}


	complex<float> operator()(std::complex<float> p) {
		return getInvertedPoint(p);
	}


	Inversion(std::complex<float> position, float radius) {
		r = radius;
		pos = position;
		circle = Circle(pos, r);
	}
	Inversion() : Inversion(complex<float>(0, 0), 1) {};

	Inversion(Circle circle) : Inversion(circle.pos, circle.r) {};
private:
	std::complex<float> getInvertedPoint(std::complex<float> p) {
		return pos + (pow(r, 2)) / conj(p - pos);
	}

};