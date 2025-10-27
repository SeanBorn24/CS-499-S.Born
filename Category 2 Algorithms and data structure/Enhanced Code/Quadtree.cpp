// ===============================
// File: Quadtree.cpp
// ===============================
#include "Quadtree.h"


static AABB childAABB(const AABB& b, int quadrant) {
// 0:nw,1:ne,2:sw,3:se (XZ plane split)
float midX = (b.min.x + b.max.x) * 0.5f;
float midZ = (b.min.z + b.max.z) * 0.5f;
AABB out = b;
switch (quadrant) {
case 0: out.max.x = midX; out.min.z = midZ; break; // NW
case 1: out.min.x = midX; out.min.z = midZ; break; // NE
case 2: out.max.x = midX; out.max.z = midZ; break; // SW
case 3: out.min.x = midX; out.max.z = midZ; break; // SE
}
return out;
}


Quadtree::Quadtree(const AABB& boundary, int capacity, int maxDepth)
: boundary_(boundary), capacity_(capacity), maxDepth_(maxDepth) {}


Quadtree::~Quadtree() { clear(); }


void Quadtree::subdivide() {
if (divided_) return;
nw_ = new Quadtree(childAABB(boundary_, 0), capacity_, maxDepth_);
ne_ = new Quadtree(childAABB(boundary_, 1), capacity_, maxDepth_);
sw_ = new Quadtree(childAABB(boundary_, 2), capacity_, maxDepth_);
se_ = new Quadtree(childAABB(boundary_, 3), capacity_, maxDepth_);


nw_->depth_ = ne_->depth_ = sw_->depth_ = se_->depth_ = depth_ + 1;
divided_ = true;
}


bool Quadtree::insert(const SceneObject& obj) {
if (!obj.bounds.intersects(boundary_)) return false;


if ((int)objects_.size() < capacity_ || depth_ >= maxDepth_) {
objects_.push_back(obj);
return true;
}


subdivide();


if (nw_->insert(obj)) return true;
if (ne_->insert(obj)) return true;
if (sw_->insert(obj)) return true;
if (se_->insert(obj)) return true;


// Fallback if none accepted (rare when bounds are larger than children)
objects_.push_back(obj);
return true;
}


void Quadtree::query(const AABB& range, std::vector<const SceneObject*>& out) const {
if (!range.intersects(boundary_)) return;
for (const auto& o : objects_) {
if (o.bounds.intersects(range)) out.push_back(&o);
}
if (!divided_) return;
nw_->query(range, out);
ne_->query(range, out);
sw_->query(range, out);
se_->query(range, out);
}


void Quadtree::clear() {
objects_.clear();
if (nw_) { nw_->clear(); delete nw_; nw_ = nullptr; }
if (ne_) { ne_->clear(); delete ne_; ne_ = nullptr; }
if (sw_) { sw_->clear(); delete sw_; sw_ = nullptr; }
if (se_) { se_->clear(); delete se_; se_ = nullptr; }
divided_ = false;
}