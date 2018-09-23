#include <iostream>
#define _USE_MATH_DEFINES

#include <vector>
#include <complex>
#include <learnopengl/shader_m.h>
#include "Inversion.h"
#include <math.h>
#include <chrono>


using namespace std;

class SuperCircle {
private:
	Inversion inversionToConcentric;
	Shader * shaderHandle;
	glCircles * circleHandle;
	float alpha;


	

public:

	float r;
	float midRatio;
	float distFromICenter;
	int recLevel;
	Circle visibleCircle;
	std::complex<float> pos;
	Circle circle;
	bool leaf;

	vector<SuperCircle> children;
	SuperCircle(std::complex<float> position, float radius, Shader * shader, glCircles * circles, int level) {
		shaderHandle = shader;
		circleHandle = circles;
		recLevel = level;
		leaf = true;
		r = radius;
		pos = position;
		circle = Circle(pos, r);
		visibleCircle.setRandomColor(recLevel);

		float random1 = 2 * glm::pi<float>() * float(rand()) / float(RAND_MAX);
		float random2 = 1.1f + 1.0f * float(rand()) / float(RAND_MAX);
		inversionToConcentric = Inversion((cos(random1) + sin(random1) * 1.0if) * random2, 1.0f);

	}
	SuperCircle(Circle circ, Shader * shader, glCircles * circles, int level = 0) : SuperCircle(circ.pos, circ.r, shader, circles, level = level) {};
	SuperCircle(float midReal, float midImag, float r, Shader * shader, glCircles * circles, int level = 0) : SuperCircle(complex<float>(midReal, midImag), r, shader, circles, level = level) {};


	void createChildren(int n) {
		children = vector<SuperCircle>();
		leaf = true;
		// "i" stands for inverted

		if (n < 3)
			return;
		leaf = false;
		Circle unitCircle;

		Circle iSuperCircle = inversionToConcentric(unitCircle);

		// The ratio of the radii of two concentric circles with a chain of "n" circles around them
		float ratio = (1.0f - sinf(M_PI / float(n))) / (1.0f + sinf(M_PI / float(n)));

		// Create middle child and push as first element in list
		Circle iMidSubCircle(iSuperCircle.pos, ratio * iSuperCircle.r);
		SuperCircle midChild(inversionToConcentric(iMidSubCircle), shaderHandle, circleHandle);
		//iMidSubCircle.printInfo();
		children.push_back(midChild);


		for (int i = 0; i < n; i++) {
			//						  Direction								Radius
			complex<float> iRelPos = exp(float(i) * 1.0if * float(2 * M_PI / n)) * (iSuperCircle.r + iMidSubCircle.r) / 2.0f;
			complex<float> iPos = iRelPos + iSuperCircle.pos;
			float iR = (iSuperCircle.r - iMidSubCircle.r) / 2.0f;

			Circle iCircle(iPos, iR);

			SuperCircle child(inversionToConcentric(iCircle), shaderHandle, circleHandle, recLevel + 1);
			children.push_back(child);
		}
		setMidRatio();
	}

	void moveInversionTo(std::complex<float> position) {
		translateInversion(position - inversionToConcentric.pos);
	}

	void translateInversion(std::complex<float> translation) {
		inversionToConcentric.pos += translation;

	}
	void setMidRatio() {
		midRatio = (1.0f - sinf(M_PI / float(children.size() - 1))) / (1.0f + sinf(M_PI / float(children.size() - 1)));
	}
	void setVisibleCircle(std::complex<float> position, float radius) {
		visibleCircle.pos = position;
		visibleCircle.r = radius;

		// Optimization
		if (visibleCircle.r < 1.0f)
			return;
		//test.draw(circleHandle, shaderHandle);
		for (SuperCircle &child : children) {
			child.setVisibleCircle(position + radius * child.pos, radius * child.r);
		}
	}

	void move(std::complex<float> rotation) {
		if (leaf)
			return;
		//Optimization
		if (visibleCircle.r < 1.0f)
			return;
		//

		Circle invertedOuter = inversionToConcentric(Circle());

		setDistanceFromInvertedCenter();
		float distanceFromInvertedCenter = getDistanceFromInvertedCenter();

		int childN = children.size() - 1;
		complex<float> rotationStep = cosf(2 * M_PI / float(childN)) + sinf(2 * M_PI / float(childN)) * 1.0if;

		complex<float> angleVector = rotation * distanceFromInvertedCenter * (inversionToConcentric(children[1].circle).pos - invertedOuter.pos) / abs(inversionToConcentric(children[1].circle).pos - invertedOuter.pos);

		for (SuperCircle &child : children) {


			Circle iCurrentCircle = inversionToConcentric(child.circle);

			if (&child == &children.front()) {
				iCurrentCircle.pos = invertedOuter.pos;
				iCurrentCircle.r = invertedOuter.r * getMidRatio();
				child.setCircle(inversionToConcentric(iCurrentCircle));
				child.move(conj(rotation));
				continue;
			}

			Circle iNewCircle = Circle(invertedOuter.pos + angleVector, distanceFromInvertedCenter - (getMidRatio() * invertedOuter.r));
			Circle newCircle = inversionToConcentric(iNewCircle);

			child.setCircle(newCircle);
			child.move(conj(rotation));
			angleVector *= rotationStep;
		}
	}
	void move(float angle) { move(cosf(angle) + 1.0if * sinf(angle)); }
	void createRecursiveChildren(int levels, int perLevel, bool mid) {

		if (levels == 0) {
			return;
		}
		createChildren(perLevel);
		for (SuperCircle &child : children) {
			if (&child != &children.front() || mid)
				child.createRecursiveChildren(levels - 1, perLevel, mid);
		}
	}

	void setCircle(std::complex<float> position, float radius) {
		pos = position;
		r = radius;
		circle.pos = pos;
		circle.r = r;
	}
	void setCircle(Circle circ) { setCircle(circ.pos, circ.r); }

	float getMidRatio() {
		if (!midRatio)
			setMidRatio();
		return midRatio;
	}
	float getDistanceFromInvertedCenter() {
		setDistanceFromInvertedCenter();
		return distFromICenter;
	}
	void setDistanceFromInvertedCenter() {
		distFromICenter = inversionToConcentric(Circle()).r * (1.0f + getMidRatio()) / 2.0f;
	}

	void draw() {
		if (visibleCircle.r < 1.0f)
			return;
		/*
		if (leaf)
			visibleCircle.draw(circleHandle, shaderHandle, true);
		else
		*/
		glLineWidth(visibleCircle.r / 200.0f + 1.0f);
		visibleCircle.draw(circleHandle, shaderHandle, false);
		for (SuperCircle &child : children) {
			child.draw();
		}

	}

	void moveMiddle(std::complex<float> to) {
		float r = abs(to - visibleCircle.pos);
		if (r > visibleCircle.r * 9 / 10) {
			r = visibleCircle.r * 9 / 10; 
		}

		std::complex<float> rotation = ((to - visibleCircle.pos) / abs(to - visibleCircle.pos));


		Circle testCircleOuter(0.0f, 1.0f);
		Circle testCircleInner(0.0f, getMidRatio());
		Inversion testInversion(0.0f, 1.0f);

		float desiredNormalizedDistance = r / visibleCircle.r;

		float l = 1.0f, h = 100.0f;
		while (h - l > 0.00001f) {
			testInversion.pos = (l + h) / 2;
			Circle outer = testInversion(testCircleOuter);
			//cout << (outer.pos.real() - testInversion(testCircleInner).pos.real()) / outer.r << endl;
			float res = (abs(outer.pos.real() - testInversion(testCircleInner).pos.real()) / outer.r);
			//cout << desiredNormalizedDistance - res << endl;
			if (res < desiredNormalizedDistance) {
				h = (l + h) / 2.0f;
			}
			else {
				l = (l + h) / 2.0f;
			}

		}
		testInversion.pos *= rotation;



		Circle newInvertedUnitCircle = testInversion(Circle());

		std::complex<float> arg1 = newInvertedUnitCircle.pos;
		float arg2 = 1.0f / newInvertedUnitCircle.r;

		Circle newInversionCircle = Circle(testInversion.pos, testInversion.r);

		newInversionCircle.scaleFromPoint(arg1, arg2);
		testInversion.pos = newInversionCircle.pos;
		testInversion.r = newInversionCircle.r;

		std::complex<float> delTranslate = arg1;

		std::complex<float> translation1 = inversionToConcentric(Circle()).pos;
		float scale1 = 1.0f / inversionToConcentric(Circle()).r;






		for (SuperCircle &child : children) {

			//Invert circle to concentric and normalize
			Circle inverted = inversionToConcentric(child.circle);
			inverted.pos -= translation1;
			inverted.scaleFromPoint(0.0f, scale1);

			//Circle is now normalized


			inverted.scaleFromPoint(arg1, arg2);
			//Circle is now in the state which allows it to be inverted to a circle with desired radius

			Circle newCirc = testInversion(inverted);

			newCirc.pos -= delTranslate;
			//newCirc.printInfo();
			child.setCircle(newCirc);
		}
		//cout << endl;

		testInversion.pos -= delTranslate;
		inversionToConcentric = testInversion;
		//cout << abs(children[0].pos) << " " << desiredNormalizedDistance << endl;

	}
};