#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "sceneelement.h"
#include "transformnode.h"
#include "boundingbox.h"

class BoundingBox;

// A Geometry object is anything that has extent in three dimensions.
// It may not be an actual visible scene object.  For example, hierarchical
// spatial subdivision could be expressed in terms of Geometry instances.
class Geometry
	: public SceneElement
{
public:
	// intersections performed in the global coordinate space.
	virtual bool intersect(const ray& r, isect& i) const;

	// intersections performed in the object's local coordinate space
	// do not call directly - this should only be called by intersect()
	virtual bool intersectLocal(const ray& r, isect& i) const;


	virtual bool hasBoundingBoxCapability() const;
	BoundingBox getBoundingBox() const;

	/// <summary>
	/// Axis-aligned bounding box (AABB)
	/// Called before add to the scene, feel free to use bound in raytracing.
	/// </summary>
	virtual void ComputeBoundingBox();

	// default method for ComputeLocalBoundingBox returns a bogus bounding box;
	// this should be overridden if hasBoundingBoxCapability() is true.
	virtual BoundingBox ComputeLocalBoundingBox();

	void setTransform(TransformNode* transform);

	Geometry(Scene* scene);

protected:
	// bound in global space.
	BoundingBox bounds;
	TransformNode* transform;
};

#endif //__GEOMETRY_H__