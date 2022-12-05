#include <cmath>

#include "scene.h"
#include "../ui/TraceUI.h"


extern TraceUI* traceUI;

Scene::~Scene()
{
    giter g;
    liter l;
    
	for( g = objects.begin(); g != objects.end(); ++g ) {
		delete (*g);
	}

	for( g = boundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( g = nonboundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( l = lights.begin(); l != lights.end(); ++l ) {
		delete (*l);
	}
}

void Scene::add(Geometry* obj)
{
	obj->ComputeBoundingBox();
	objects.push_back(obj);
}

void Scene::addAmbientLight(vec3f color)
{
	if (ambientLight == nullptr)
	{
		ambientLight = new AmbientLight(this, color);
	}
	else
	{
		ambientLight->add(color);
	}
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const
{
	typedef list<Geometry*>::const_iterator iter;
	iter j; // dereference j is (Geometry*)

	isect cur;
	bool have_one = false;

	// try the non-bounded objects
	for (j = nonboundedobjects.begin(); j != nonboundedobjects.end(); ++j) {
		if ((*j)->intersect(r, cur)) {
			if (!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
	}

	// try the bounded objects
	switch (traceUI->getSpatialStructure())
	{
	case SpatialType::Default:
		for (j = boundedobjects.begin(); j != boundedobjects.end(); ++j) {
			if ((*j)->intersect(r, cur)) {
				if (!have_one || (cur.t < i.t)) {
					i = cur;
					have_one = true;
				}
			}
		}
		break;

	case SpatialType::BSP:
		if (bsproot->intersect(r, cur))
		{
			i = cur;
			have_one = true;
		}
		break;

	default:
		for (j = boundedobjects.begin(); j != boundedobjects.end(); ++j) {
			if ((*j)->intersect(r, cur)) {
				if (!have_one || (cur.t < i.t)) {
					i = cur;
					have_one = true;
				}
			}
		}
		break;
	}

	return have_one;
}

void Scene::initScene()
{
	bool first_boundedobject = true;
	BoundingBox b;
	typedef list<Geometry*>::const_iterator iter;

	// split the objects into two categories: bounded and non-bounded
	for (iter j = objects.begin(); j != objects.end(); ++j) {
		if ((*j)->hasBoundingBoxCapability())
		{
			boundedobjects.push_back(*j);

			// widen the scene's bounding box, if necessary
			if (first_boundedobject) {
				sceneBounds = (*j)->getBoundingBox();
				first_boundedobject = false;
			}
			else
			{
				b = (*j)->getBoundingBox();
				sceneBounds.mergeBoundingBox(b);
			}
		}
		else
			nonboundedobjects.push_back(*j);
	}

	// construct bsp tree with boundedobjects
	bsproot = std::make_shared<BSPNode>(this, boundedobjects);
}

vec3f Scene::getAmbientColor()
{
	return (ambientLight) ? ambientLight->getColor() : vec3f(0.0, 0.0, 0.0);
}

