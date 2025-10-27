// ===============================
// File: ViewManager.cpp (existing with tiny doc/robustness tweaks)
// ===============================
#include "ViewManager.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


namespace {
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const char* g_ViewName = "view";
const char* g_ProjectionName = "projection";


Camera* g_pCamera = nullptr;
float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;
float gDeltaTime = 0.0f;
float gLastFrame = 0.0f;
}


ViewManager::ViewManager(ShaderManager* pShaderManager) {
m_pShaderManager = pShaderManager;
m_pWindow = nullptr;
g_pCamera = new Camera();
g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
g_pCamera->Zoom = 80;
}


ViewManager::~ViewManager() {
m_pShaderManager = nullptr;
m_pWindow = nullptr;
if (g_pCamera) { delete g_pCamera; g_pCamera = nullptr; }
}


GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle) {
GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
if (!window) {
std::cout << "Failed to create GLFW window" << std::endl;
glfwTerminate();
return nullptr;
}
glfwMakeContextCurrent(window);
glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);


glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


m_pWindow = window;
return window;
}


void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double x, double y) {
if (gFirstMouse) { gLastX = (float)x; gLastY = (float)y; gFirstMouse = false; }
float xOffset = (float)x - gLastX;
float yOffset = gLastY - (float)y;
gLastX = (float)x; gLastY = (float)y;
if (g_pCamera) g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}


void ViewManager::ProcessKeyboardEvents() {
if (!m_pWindow) return;
if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(m_pWindow, true);
}