#include "spatialnode.h"
#include "scene.h"

BSPNode::BSPNode(Scene* scene, const list<Geometry*> objs)
	: SpatialNode(scene), plane(nullptr), front(nullptr), back(nullptr)
{
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
	: SpatialNode(scene), region(std::move(box)), isEmpty(true)
{

}

void OctNode::buildTree(list<Geometry*> objs)
{
	using std::cout;
	using std::endl;

	if (objs.size() == 0)
	{

	}
	else if (objs.size() <= 1)
	{
		isEmpty = false;
		typedef list<Geometry*>::const_iterator citer;
		for (citer obj = objs.cbegin(); obj != objs.cend(); ++obj)
		{
			geometries.push_back(*obj);
		}
	}
	else
	{
		// Set up sub-regions
		vec3f center = (region.max + region.min) / 2;
		//std::cout << "max: " << region.max << "\tmin: " << region.min << "\tcenter: " << center << std::endl;
		vec3f rt(region.max[0], center[1], region.max[2]);
		vec3f b(center[0], region.min[2], center[2]);
		vec3f t(center[0], center[1], region.max[2]);
		vec3f lb(region.min[0], region.min[1], center[2]);
		vec3f ft(center[0], region.max[1], region.max[2]);
		vec3f l(region.min[0], center[1], center[2]);
		vec3f rf(region.max[0], region.max[1], center[2]);
		vec3f d(center[0], center[1], region.min[2]);
		vec3f r(region.max[0], center[1], center[2]);
		vec3f bd(center[0], region.min[1], region.min[2]);
		vec3f f(center[0], region.max[1], center[2]);
		vec3f ld(region.min[0], center[1], region.min[2]);
		//std::cout << "f: " << f << "\tl: " << l << "\td: " << d << std::endl;
		//std::cout << "r: " << r << "\tt: " << t << "\tb: " << b << std::endl;
		//std::cout << "ft: " << ft << "\tlb: " << lb << "\tbd: " << bd << std::endl;

		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(region.max, center))); // RFT
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(rt, b))); // RBT
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(t, lb))); // LBT
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(ft, l))); // LFT
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(rf, d))); // RFD
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(r, bd))); // RBD
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(center, region.min))); // LBD
		children.push_back(std::make_shared<OctNode>(scene, BoundingBox(f, ld))); // LFD

		for (Geometry* obj : objs)
		{
			const BoundingBox& objBox = obj->getBoundingBox();
			vector<OctNode*> collisions;
		
			for (shared_ptr<OctNode> node : children)
			{
				if (objBox.intersects(node->region))
				{
					collisions.push_back(node.get());
				}
			}

			if (collisions.size() == 0)
			{
				cout << "error: no bounding box region found." << endl;
				return;
			}
			// intersect with multiple regions -> put in this node.
			else if (collisions.size() > 1)
			{
				geometries.push_back(obj);
			}
			// intersect with one region.
			else
			{
				collisions[0]->tempObjs.push_back(obj);
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			children[i]->buildTree(children[i]->tempObjs);
		}
	}

	// clean up
	tempObjs.clear();
}

bool OctNode::intersect(const ray& r, isect& i)
{
	if (IsEmpty())
		return false;

	isect cur;
	typedef list<Geometry*>::iterator iter;
	iter j;
	bool have_one = false;

	// iterate through internal node
	for (iter iter = geometries.begin(); iter != geometries.end(); ++iter)
	{
		if ((*iter)->intersect(r, cur))
		{
			if (!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
	}

	if (!IsLeaf())
	{
		for (int ii = 0; ii < 8; ++ii)
		{
			// intersect with sub region boundingbox
			double tmin, tmax;
			if (children[ii]->region.intersect(r, tmin, tmax))
			{
				if (children[ii]->intersect(r, cur))
				{
					if (!have_one || (cur.t < i.t)) {
						i = cur;
						have_one = true;
					}
				}
			}
		}
	}

	return have_one;
}