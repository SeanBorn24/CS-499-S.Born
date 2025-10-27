// ===============================
// File: ApplicationManager.cpp
// ===============================
#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "ApplicationManager.h"


ApplicationManager::ApplicationManager() {}
ApplicationManager::~ApplicationManager() { Shutdown(); }


bool ApplicationManager::Initialize(const std::string& windowTitle) {
if (!InitializeGLFW()) return false;


shaderManager_ = std::make_unique<ShaderManager>();
viewManager_ = std::make_unique<ViewManager>(shaderManager_.get());


// Create window via ViewManager
window_ = viewManager_->CreateDisplayWindow(windowTitle.c_str());
if (!window_) return false;


if (!InitializeGLEW()) return false;


// Load and activate shaders
shaderManager_->LoadShaders(
"../../Utilities/shaders/vertexShader.glsl",
"../../Utilities/shaders/fragmentShader.glsl");
shaderManager_->use();


// Prepare scene
sceneManager_ = std::make_unique<SceneManager>(shaderManager_.get());
sceneManager_->PrepareScene();


return true;
}


void ApplicationManager::Run() {
if (!window_ || !sceneManager_ || !viewManager_) return;


while (!glfwWindowShouldClose(window_)) {
glEnable(GL_DEPTH_TEST);
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


viewManager_->PrepareSceneView();
sceneManager_->RenderScene();


glfwSwapBuffers(window_);
glfwPollEvents();
}
}


void ApplicationManager::Shutdown() {
sceneManager_.reset();
viewManager_.reset();
shaderManager_.reset();
if (window_) {
// Do not destroy here; GLFW handles in user code if desired
window_ = nullptr;
}
}


bool ApplicationManager::InitializeGLFW() {
glfwInit();
#ifdef __APPLE__
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);