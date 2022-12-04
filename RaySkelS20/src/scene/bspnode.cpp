#include "bspnode.h"



BSPNode::BSPNode(Scene* scene, list<Geometry*> objs)
	: scene(scene), plane(nullptr)
{
	std::cout << "hello\n";
	if (objs.size() <= 1)
	{
		geometries = std::move(objs);
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

			// Init plane
			vec3f pn = Plane::GenerateRandomNormal();
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

	// no plane => is leaf
	if (plane == nullptr)
	{
		for (j = geometries.begin(); j != geometries.end(); ++j)
		{
			if ((*j)->intersect(r, cur))
			{
				i = cur;
				return true;
			}
		}

		return false;
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
	for (j = geometries.begin(); j != geometries.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			i = cur;
			return true;
		}
	}

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
	for (j = geometries.begin(); j != geometries.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			i = cur;
			return true;
		}
	}

	// front
	if (front && front->intersect(r, i))
		return true;

	return false;
}
