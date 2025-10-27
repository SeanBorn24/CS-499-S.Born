// ===============================
// File: tests/ResourceLoadingTests.cpp
// ===============================
#include <cassert>
#include "SceneManager.h"
#include "ShaderManager.h"


int main() {
ShaderManager dummy; // assumes default-constructible for test harness
SceneManager sm(&dummy);
// We won't actually load textures (GL context required),
// but we can at least call DefineObjectMaterials safely.
sm.DefineObjectMaterials();
// If no crash, consider pass for smoke test.
return 0;
}