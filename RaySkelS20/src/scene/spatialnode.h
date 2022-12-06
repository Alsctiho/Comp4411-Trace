#pragma once

#include <vector>
#include <memory>
#include "geometry.h"
#include "ray.h"
#include "../NonSceneObjects/Plane.h"

enum
{
	RFT, // right-front-top
	RBT,
	LBT,
	LFT,
	RFD, // right-front-down
	RBD,
	LBD, // left-back-down
	LFD,
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
/// 
/// </summary>
class OctNode : public SpatialNode
{
private:
	bool isEmpty;
	BoundingBox region;
	list<Geometry*> tempObjs; // Only used in buildTree.
	list<Geometry*> geometries;
	vector<shared_ptr<OctNode>> children;

public:
	bool IsEmpty() const { return IsLeaf() && isEmpty; }
	bool IsLeaf() const { return children.size() == 0; }

	OctNode(Scene* scene, BoundingBox box);
	void buildTree(list<Geometry*> objs);
	virtual bool intersect(const ray& r, isect& i);
};