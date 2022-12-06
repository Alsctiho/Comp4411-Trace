#pragma once

#include <vector>
#include <memory>
#include "geometry.h"
#include "ray.h"
#include "../NonSceneObjects/Plane.h"

enum
{

};

using std::list;
using std::shared_ptr;
using std::vector;

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

class BSPNode : public SpatialNode
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

/// <summary>
/// ref: https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/
/// </summary>
class OctNode : public SpatialNode
{
private:
	BoundingBox region;
	list<Geometry*> geometries;
	vector<shared_ptr<BSPNode>> children;

public:
	OctNode(Scene* scene, BoundingBox box);
	void buidTree(list<Geometry*> objs);
	virtual bool intersect(const ray& r, isect& i);
};