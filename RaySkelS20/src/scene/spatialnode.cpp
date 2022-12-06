#include "spatialnode.h"
#include "scene.h"

BSPNode::BSPNode(Scene* scene, const list<Geometry*> objs)
	: SpatialNode(scene), plane(nullptr), front(nullptr), back(nullptr)
{
	//std::cout << "hello\n";
	if (objs.size() <= 1)
	{
		typedef list<Geometry*>::const_iterator citer;
		for (citer obj = objs.cbegin(); obj != objs.cend(); ++obj)
		{
			geometries.push_back(*obj);
		}
	}
	else
	{
		list<Geometry*> frontList, backList;
		vec3f pp = objs.front()->getBoundingBox().min;

		// make sure both child have at least one child.
		do
		{
			frontList.clear();
			backList.clear();
			geometries.clear();

			// Init plane
			vec3f pn = Plane::GenerateRandomNormal();
			//vec3f pp = scene->getSceneBoundingBox().GenerateRandomPoint();
			plane = std::make_shared<Plane> (pn, pp);

			// test half space
			for (Geometry* obj : objs)
			{
				int res = plane->TestHalfSpace(obj);
				if (res > 0)
				{
					frontList.push_back(obj);
				}
				else if (res < 0)
				{
					backList.push_back(obj);
				}
				else
				{
					geometries.push_back(obj);
				}
			}
		} while (
			(frontList.size() == 0 && geometries.size() == 0) ||
			(geometries.size() == 0 && backList.size() == 0));

		// construct subtree front and back.
		if (frontList.size())	front = std::make_shared<BSPNode>(scene, frontList);
		if (backList.size())	back = std::make_shared<BSPNode>(scene, backList);
	}
}

bool BSPNode::intersect(const ray& r, isect& i)
{
	isect cur;
	typedef list<Geometry*>::const_iterator iter;
	iter j;
	bool have_one = false;

	// no plane => is leaf
	if (front == nullptr && back == nullptr)
	{
		for (j = geometries.begin(); j != geometries.end(); ++j)
		{
			if ((*j)->intersect(r, cur))
			{
				if (!have_one || (cur.t < i.t)) {
					i = cur;
					have_one = true;
				};
			}
		}

		return have_one;
	}
	// has plane => is parent
	else
	{
		// if ray and N is in same direction
		if (plane->TestSameDirection(r.getDirection()))
		{
			// back to front
			return intersectBack(r, i);
		}
		else
		{
			// front to back
			return intersectFront(r, i);
		}
	}
}

bool BSPNode::intersectFront(const ray& r, isect& i)
{
	isect cur;
	typedef list<Geometry*>::const_iterator iter;
	iter j;

	// front
	if (front && front->intersect(r, i))
		return true;


	// current
	bool have_one = false;
	for (j = geometries.begin(); j != geometries.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			if (!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
	}

	if (have_one)
		return true;

	// back
	if (back && back->intersect(r, i))
		return true;

	return false;
}

bool BSPNode::intersectBack(const ray& r, isect& i)
{
	isect cur;
	typedef list<Geometry*>::const_iterator iter;
	iter j;

	// back
	if (back && back->intersect(r, i))
		return true;

	// current
	bool have_one = false;
	for (j = geometries.begin(); j != geometries.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			if (!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
	}

	if (have_one)
		return true;

	// front
	if (front && front->intersect(r, i))
		return true;

	return false;
}

OctNode::OctNode(Scene* scene, BoundingBox box)
	: SpatialNode(scene), region(box)
{

}

void OctNode::buidTree(list<Geometry*> objs)
{

}

bool OctNode::intersect(const ray& r, isect& i)
{
	return false;
}