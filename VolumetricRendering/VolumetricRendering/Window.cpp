#include "Window.h"

#include "Window.h"

unsigned int Window::SCR_WIDTH = 0;
unsigned int Window::SCR_HEIGHT = 0;

Camera* Window::camera = 0;
bool Window::m_keys[10] = { false, false,false, false, false, false, false, false, false, false };
bool Window::m_wireframe = false;
bool Window::m_firstMouse = true;
float Window::m_lastX = SCR_WIDTH / 2.0f;
float Window::m_lastY = SCR_HEIGHT / 2.0f;

bool Window::m_mouseCursorDisabled = true;

Window::Window(bool& success, unsigned int scrW, unsigned int scrH, std::string name) : m_name{ name }
{

	Window::SCR_WIDTH = scrW;
	Window::SCR_HEIGHT = scrH;
	success = true;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
									 
	this->w = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.c_str(), NULL, NULL);
	if (!this->w)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		success = false;
		return;
	}
	glfwMakeContextCurrent(this->w);
	glfwSetFramebufferSizeCallback(this->w, &Window::framebuffer_size_callback);
	glfwSetCursorPosCallback(this->w, &Window::mouse_callback);
	glfwSetScrollCallback(this->w, &Window::scroll_callback);

	Window::camera = 0;
	oldState = newState = GLFW_RELEASE;

	success = gladLoader() && success;
	if (success) {
		std::cout << "GLFW window correctly initialized!" << std::endl;
	}
}

bool Window::gladLoader() {

	glfwSetInputMode(this->w, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	return true;
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (m_firstMouse)
	{
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstMouse = false;
	}

	float xoffset = xpos - m_lastX;
	float yoffset = m_lastY - ypos;

	m_lastX = xpos;
	m_lastY = ypos;
	if (!m_mouseCursorDisabled)
		Window::camera->ProcessMouseMovement(xoffset, yoffset);
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window::camera->ProcessMouseScroll(yoffset);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Window::processInput(float frameTime) {
	if (glfwGetKey(this->w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->w, true);

	if (glfwGetKey(this->w, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, frameTime);
	if (glfwGetKey(this->w, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, frameTime);
	if (glfwGetKey(this->w, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, frameTime);
	if (glfwGetKey(this->w, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, frameTime);

	newState = glfwGetMouseButton(this->w, GLFW_MOUSE_BUTTON_RIGHT);

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
		glfwSetInputMode(this->w, GLFW_CURSOR, (m_mouseCursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
		m_mouseCursorDisabled = !m_mouseCursorDisabled;
		if (m_mouseCursorDisabled) {
			m_firstMouse = true;
		}
	}

	oldState = newState;

	if (glfwGetKey(this->w, GLFW_KEY_T) == GLFW_PRESS) {
		if (m_keys[4] == false) {
			m_wireframe = !m_wireframe;
			m_keys[4] = true;
		}
	}
	else if (glfwGetKey(this->w, GLFW_KEY_T) == GLFW_RELEASE) {
		if (m_keys[4] == true) { m_keys[4] = false; }
	}
}

Window::~Window()
{
	this->terminate();
}

void Window::terminate() {
	glfwTerminate();
}

bool Window::isWireframeActive() {
	return Window::m_wireframe;
}

bool Window::continueLoop() {
	return !glfwWindowShouldClose(this->w);
}

void Window::swapBuffersAndPollEvents() {
	glfwSwapBuffers(this->w);
	glfwPollEvents();
}