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

class SpatialNode
{
protected:
	Scene* scene;

public:
	SpatialNode(Scene* scene) : scene(scene) {}
	virtual bool intersect(const ray& r, isect& i) = 0;
};

class BSPNode : SpatialNode
{
private:
	shared_ptr<Plane> plane;

	list<Geometry*> geometries;
	shared_ptr<BSPNode> front;
	shared_ptr<BSPNode> back;

public:
	BSPNode(Scene* scene, const list<Geometry*> objs);
	virtual bool intersect(const ray& r, isect& i);
	bool intersectFront(const ray& r, isect& i);
	bool intersectBack(const ray& r, isect& i);
};

class OctNode : SpatialNode
{
public:
	OctNode(Scene* scene, const list<Geometry*> objs);
	virtual bool intersect(const ray& r, isect& i);
};