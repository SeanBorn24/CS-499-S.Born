// ===============================
// File: tests/MatrixTests.cpp
// ===============================
#include <cassert>
#include <glm/glm.hpp>


int main() {
glm::mat4 I(1.0f);
glm::mat4 A = I * I;
assert(A == I);
return 0;
}
