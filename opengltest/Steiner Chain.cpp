

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <complex>
#include "SuperCircle.h"
#include <chrono>
#include <learnopengl/shader_m.h>
#include <iostream>
#include <algorithm>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void move_selected(GLFWwindow * window);
void character_callback(GLFWwindow* window, unsigned int codepoint);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float dt = 0;
bool PAUSED = false;
float speed = 0.2f;
int CIRCLES_ADD = 12;
bool RECURSE_MID = false;
bool SYNC_CHILDREN = false;

SuperCircle * hit;
glCircles circles;
SuperCircle * root;
SuperCircle * running;


std::complex<float> normalize(std::complex<float> x) {
	return x / abs(x);
}

GLFWwindow * generateWindow() {
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Steiners porism", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, processInput);
	glfwSetCharCallback(window, character_callback);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	return window;
}



int loadGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	return 0;
}



int main()
{

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = generateWindow();

	loadGlad();


	Shader shader(
		"circleVert.vert",
		"circleFrag.frag"
	);



	// Pre-generate levels of detail
	circles.generateLevelsOfDetail(100, 20, 1000);
	circles.generateQuad();

	// Test circle
	Circle test;
	test.pos = 0.0 + 0.0i;
	test.r = 100.0f;

	root = &SuperCircle(0.0f + 0.0if, 100.0f, &shader, &circles, 0);
	running = root;
	root->createRecursiveChildren(1, 14, true);




	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float lt = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{

		dt = glfwGetTime() - lt;
		if (dt < 0.016666f)
			continue;

		lt = glfwGetTime();


		// input
		// -----
		move_selected(window);



		//processInput(window);
		circles.setScreenDims((float)SCR_WIDTH, (float)SCR_HEIGHT);



		// render
		// ------
		glClearColor(0.05f, 0.0f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		//std::cout << int(timer / 1000) << "ms Move time" << std::endl;

		
		//std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();

		if (!PAUSED)
			running->move(speed * dt, SYNC_CHILDREN);

		//std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
		//std::cout << "Setting visible Circle: " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count() << "ms" << std::endl;
		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		//root.drawNaive(&shaderNaive);
		//root.drawNaive(&shaderNaive);
		//test.drawNaive(&circles, &shaderNaive);
		root->draw();
		//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//std::cout << "Drawing Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
		root->setVisibleCircle(0.0 + 0.0i, std::fminf(SCR_HEIGHT, SCR_WIDTH) / 3);
	}



	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (hit) {
			running = hit;
			PAUSED = !PAUSED;
		}
		else {
			PAUSED = !PAUSED;
			running = root;
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		speed *= 2.0f;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		speed *= 0.5f;
	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
		if (hit) {
			hit->createChildren(0);
			hit = NULL;
		}
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		SYNC_CHILDREN = !SYNC_CHILDREN;
	}

}
void character_callback(GLFWwindow* window, unsigned int codepoint) {
	if (codepoint == 48)
		root->createChildren(0);
	if (codepoint < 49 || codepoint > 54)
		return;
	//cout << codepoint - 48 << " " << int(15.0f * float(rand()) / float(RAND_MAX)) << endl;
	root->createRecursiveChildren(codepoint - 48, max(int(15.0f * float(rand()) / float(RAND_MAX)), 3), RECURSE_MID);
	hit = NULL;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void changeSelection(float x, float y) {

	SuperCircle * lastHit = root;
	hit = NULL;
	while (!lastHit->leaf) {
		bool hitChild = false;
		if (!hit && abs(lastHit->children[0].visibleCircle.pos - x - y * 1.0if) < 20.0f)
			hit = lastHit;
		else if (abs(lastHit->children[0].visibleCircle.pos - x - y * 1.0if) < lastHit->children[0].visibleCircle.r) {
			hit = lastHit;
		}
		for (SuperCircle &child : lastHit->children) {
			if (abs(child.visibleCircle.pos - x - y * 1.0if) < child.visibleCircle.r) {
				hitChild = true;
				lastHit = &child;
				break;
			}
		}
		if (!hitChild)
			break;
	}
}
SuperCircle * findHit(float x, float y) {
	SuperCircle * lastHit = root;
	
	while (!lastHit->leaf) {
		DBREAK:
		for (SuperCircle &child : lastHit->children) {
			if (abs(child.visibleCircle.pos - x - y * 1.0if) < child.visibleCircle.r) {
				lastHit = &child;
				goto DBREAK;
			}
		}
		break;
	}
	return lastHit;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos -= SCR_WIDTH / 2.0;
	ypos = SCR_HEIGHT / 2.0 - ypos;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		changeSelection(xpos, ypos);
		if (SYNC_CHILDREN && hit != root)
			hit = root;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		hit = NULL;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		SuperCircle * hit_create = findHit(xpos, ypos);
		if (hit_create->leaf) {
			hit_create->createChildren(CIRCLES_ADD);
			hit_create->moveMiddle(float(xpos) + 1.0if * float(ypos));
		}
	}
}


void move_selected(GLFWwindow *window) {

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos -= SCR_WIDTH / 2.0;
	ypos = SCR_HEIGHT / 2.0 - ypos;

	if (hit == NULL) {
		return;
	}
	if (abs(hit->children.front().visibleCircle.pos - (float(xpos) + float(ypos) * 1.0if)) < 1.0f) {
		return;
	}
	std::complex<float> direction = normalize((float(xpos) + float(ypos) * 1.0if) - hit->children.front().visibleCircle.pos);
	float dist = abs(hit->children.front().visibleCircle.pos - (float(xpos) + float(ypos) * 1.0if));

	hit->moveMiddle(hit->children.front().visibleCircle.pos + max(dt * dist, min(10.0f, dist)) * direction);

}