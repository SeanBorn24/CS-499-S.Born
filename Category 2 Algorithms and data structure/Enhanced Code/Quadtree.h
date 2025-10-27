// ===============================
// File: Quadtree.h
// ===============================
#pragma once
#include <vector>
#include <functional>
#include <glm/glm.hpp>


/**
* @brief Axis-aligned bounding box in XZ plane with Y extents kept for completeness.
*/
struct AABB {
glm::vec3 min; // (x,y,z)
glm::vec3 max; // (x,y,z)
bool intersects(const AABB& other) const {
return (min.x <= other.max.x && max.x >= other.min.x) &&
(min.y <= other.max.y && max.y >= other.min.y) &&
(min.z <= other.max.z && max.z >= other.min.z);
}
bool contains(const glm::vec3& p) const {
return p.x >= min.x && p.x <= max.x &&
p.y >= min.y && p.y <= max.y &&
p.z >= min.z && p.z <= max.z;
}
};


/**
* @brief Scene object handle used by the quadtree. Holds a bounding box and a draw callback.
*/
struct SceneObject {
AABB bounds;
// User data index or any ID can be added here if needed
std::function<void()> draw; // callback to render the object
};


class Quadtree {
public:
Quadtree(const AABB& boundary, int capacity = 8, int maxDepth = 8);
~Quadtree();


bool insert(const SceneObject& obj);
void query(const AABB& range, std::vector<const SceneObject*>& out) const;
void clear();


private:
void subdivide();


private:
AABB boundary_;
int capacity_;
int maxDepth_;
int depth_ = 0;


bool divided_ = false;
std::vector<SceneObject> objects_;


Quadtree* nw_ = nullptr; // -x, +z
Quadtree* ne_ = nullptr; // +x, +z
Quadtree* sw_ = nullptr; // -x, -z
Quadtree* se_ = nullptr; // +x, -z
};