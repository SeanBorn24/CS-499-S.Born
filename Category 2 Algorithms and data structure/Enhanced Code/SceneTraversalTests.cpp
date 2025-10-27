// ===============================
// File: tests/SceneTraversalTests.cpp
// ===============================
#include <cassert>
#include "Quadtree.h"


int main() {
AABB world{{-10,-1,-10},{10,1,10}};
Quadtree qt(world, 2, 4);
SceneObject a{ {{-1,-1,-1},{1,1,1}}, []{} };
SceneObject b{ {{5,-1,5},{6,1,6}}, []{} };
qt.insert(a); qt.insert(b);
std::vector<const SceneObject*> out;
qt.query({{-2,-1,-2},{2,1,2}}, out);
assert(out.size() == 1);
return 0;
}