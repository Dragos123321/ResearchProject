#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.h>
#include <iostream>

class Window
{
public:
	Window(bool& success, unsigned int SCR_WIDTH = 1920, unsigned int SCR_HEIGHT = 1200, std::string name = "Volumetric clouds");
	~Window();
	GLFWwindow* w;
	GLFWwindow* getWindow() const { return w; }

	void processInput(float frameTime); 

	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	void terminate();

	bool isWireframeActive();

	bool continueLoop();

	void swapBuffersAndPollEvents();

	static Camera* camera;

private:
	int oldState, newState;
	bool gladLoader();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static bool m_keys[10];

	static bool m_mouseCursorDisabled;

	static bool m_wireframe;

	static bool m_firstMouse;
	static float m_lastX;
	static float m_lastY;

	std::string m_name;
};