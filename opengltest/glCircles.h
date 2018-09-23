#include <glm/glm.hpp>
#include <complex>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <learnopengl/shader_m.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <math.h>

struct lodCircle {
	unsigned int VAO;
	int segs;
	lodCircle(unsigned int vao, int segments) {
		VAO = vao;
		segs = segments;
	}
};


class glCircles
{
private:
	int segs;
	float WIDTH = 1, HEIGHT = 1;
	std::vector<lodCircle> circles;
	int possibleMax, possibleMin;
	unsigned int VBONaive, VAONaive;

public:

	lodCircle generateCircle(int segments) {
		segs = segments;
		std::complex<float> angleChange(glm::cos(2.0f * glm::pi<float>() / segments), glm::sin(2.f * glm::pi<float>() / segments));
		std::complex<float> current(1, 0);

		std::vector<float> geometry = { 0.0f, 0.0f };

		for (int i = 0; i <= segs; i++) {
			geometry.push_back(current.real());
			geometry.push_back(current.imag());
			current *= angleChange;
		}
		geometry.push_back(1.0f);
		geometry.push_back(0.0f);

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, geometry.size() * sizeof(geometry[0]), &geometry[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		return lodCircle(VAO, segments);

	}
	glCircles() { }
	void generateLevelsOfDetail(int levelsOfDetail, int minVerts, int maxVerts) {

		possibleMax = maxVerts;
		possibleMin = minVerts;

		int stride = (maxVerts - minVerts) / levelsOfDetail;
		int currVerts = minVerts;
		for (int i = 0; i < levelsOfDetail; i++) {
			circles.push_back(generateCircle(currVerts));
			currVerts += stride;
		}
	}
	void generateQuad() {

		glGenVertexArrays(1, &VAONaive);
		glGenBuffers(1, &VBONaive);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAONaive);
		float verts[] = {
			-1.0, -1.0,
			1.0, 1.0,
			-1.0f, 1.0,

			1.0, 1.0,
			1.0, -1.0,
			-1.0, -1.0
		};
		glBindBuffer(GL_ARRAY_BUFFER, VBONaive);
		glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), verts, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	void setScreenDims(float width, float height) {
		WIDTH = width;
		HEIGHT = height;
	}
	// Takes in circle in width, height coordinates

	int bindSuitableVAO(glm::vec3 position, float radius, Shader * shader) {


		int requiredVerts = (int)(glm::pi<float>() * radius / 2.0f);
		int lodToUse;
		if (requiredVerts >= possibleMax) {
			lodToUse = (int)circles.size() - 1;
		}
		else if (requiredVerts <= possibleMin) {
			lodToUse = 0;
		}
		else {
			lodToUse = int((float(requiredVerts - possibleMin) / float(possibleMax - possibleMin)) * float(circles.size()));
		}
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(2.0 * position.x / WIDTH, 2.0 * position.y / HEIGHT, 0));
		model = glm::scale(model, glm::vec3(radius));
		model = glm::scale(model, glm::vec3(2 / (WIDTH), 2 / (HEIGHT), 0));

		shader->setMat4("model", model);
		shader->use();
		glBindVertexArray(circles[lodToUse].VAO);
		//if (300 < circles[lodToUse].segs)
			//std::cout << "Circle with radius " << radius << " rendered with " << circles[lodToUse].segs << " Segments" << std::endl;

		return circles[lodToUse].segs;
	}

	void drawFilled(glm::vec3 position, float radius, Shader * shader, glm::vec4 color = glm::vec4(1, 1, 1, 1)) {
		// Radius is in pixel coordinates
		segs = bindSuitableVAO(position, radius, shader);
		shader->setVec4("color", color);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segs + 2);

	}

	void drawBorder(glm::vec3 position, float radius, Shader * shader, glm::vec4 color = glm::vec4(1, 1, 1, 1)) {
		// Radius is in pixel coordinates
		segs = bindSuitableVAO(position, radius, shader);
		shader->setVec4("color", color);
		glDrawArrays(GL_LINE_LOOP, 1, segs + 2);
	}
	void drawNaive(Shader * naiveShader, float radius, float x, float y) {
		naiveShader->setFloat("radius", radius);
		naiveShader->setVec4("center", glm::vec4(x + WIDTH / 2, y + HEIGHT / 2, 0, 0));
		naiveShader->setFloat("thickness", 1.0f);
		naiveShader->use();

		glBindVertexArray(VAONaive);

		glDrawArrays(GL_TRIANGLES, 0, 6);

	}
};

