// ===============================
// File: MainCode.cpp (UPDATED to use ApplicationManager)
// ===============================
#include <iostream>
#include <cstdlib>
#include "ApplicationManager.h"


int main(int, char**) {
std::cout << "Created by Sean J. Born on October 5, 2025" << std::endl;
ApplicationManager app;
if (!app.Initialize("7-1 FinalProject and Milestones")) return EXIT_FAILURE;
app.Run();
app.Shutdown();
return EXIT_SUCCESS;
}