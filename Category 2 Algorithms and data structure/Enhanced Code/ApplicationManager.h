// ===============================
// File: ApplicationManager.h
// ===============================
#pragma once
#include <memory>
#include <string>
#include "GLFW/glfw3.h"
#include "ShaderManager.h"
#include "SceneManager.h"
#include "ViewManager.h"


/**
* @class ApplicationManager
* @brief Owns and orchestrates app lifecycle (init, loop, shutdown).
*
* Removes reliance on global variables (g_Window, g_SceneManager,
* g_ViewManager, g_ShaderManager) by encapsulating them as
* ApplicationManager members. Supports CO3 (design & engineering).
*/
class ApplicationManager {
public:
ApplicationManager();
~ApplicationManager();


// Initialize GLFW, window, GLEW, shaders, scene
bool Initialize(const std::string& windowTitle);


// Main loop – runs until the window closes
void Run();


// Cleanup
void Shutdown();


private:
bool InitializeGLFW();
bool InitializeGLEW();


private:
std::unique_ptr<ShaderManager> shaderManager_;
std::unique_ptr<ViewManager> viewManager_;
std::unique_ptr<SceneManager> sceneManager_;
GLFWwindow* window_ = nullptr;
};