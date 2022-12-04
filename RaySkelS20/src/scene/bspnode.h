#pragma once

#include <vector>
#include <memory>
#include "geometry.h"
#include "ray.h"
#include "../NonSceneObjects/Plane.h"

using std::list;
using std::shared_ptr;

class Plane;
class Geometry;

class BSPNode
{
private:
	Scene* scene;
	shared_ptr<Plane> plane;

	list<Geometry*> geometries;
	shared_ptr<BSPNode> front;
	shared_ptr<BSPNode> back;

public:
	BSPNode(Scene* scene, list<Geometry*> objs);
	bool intersect(const ray& r, isect& i);
	bool intersectFront(const ray& r, isect& i);
	bool intersectBack(const ray& r, isect& i);
};