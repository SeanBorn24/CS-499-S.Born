// ===============================
// File: ViewManager.h (UPDATED DOCS)
// ===============================
#pragma once
#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"


enum ProjectionType { PERSPECTIVE, ORTHOGRAPHIC };


/**
* @class ViewManager
* @brief Manages camera, projection, and per-frame view state.
*
* Exposes helpers to prepare the scene view/projection matrices
* and process input. Supports CO3 (separation of concerns) and
* CO4 (documentation).
*/
class ViewManager {
public:
/** Construct a ViewManager with an active ShaderManager. */
ViewManager(ShaderManager* pShaderManager);
/** Destructor – frees camera resources. */
~ViewManager();


/** GLFW mouse callback for orbit/pan. */
static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);


/** Create the main GLFW window and GL context. */
GLFWwindow* CreateDisplayWindow(const char* windowTitle);
/** Compute view/projection, push to shaders, and process input. */
void PrepareSceneView();


// Mouse scroll speed control
void Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessMouseScroll(double yoffset);


// Projection toggle (PERSPECTIVE <-> ORTHOGRAPHIC)
void ToggleProjection();


private:
void ProcessKeyboardEvents();


private:
ShaderManager* m_pShaderManager = nullptr; // non-owning
GLFWwindow* m_pWindow = nullptr; // active window
float cameraSpeed = 0.05f; // adjustable via scroll
ProjectionType currentProjection = PERSPECTIVE; // default
};