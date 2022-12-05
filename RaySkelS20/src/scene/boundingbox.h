#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__

#include "ray.h"
#include "..\vecmath\vecmath.h"

class BoundingBox
{
public:
	vec3f min;
	vec3f max;

	void operator=(const BoundingBox& target);

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox& target) const;

	// does the box contain this point?
	bool intersects(const vec3f& point) const;

	// if the ray hits the box, put the "t" value of the intersection
	// closest to the origin in tMin and the "t" value of the far intersection
	// in tMax and return true, else return false.
	bool intersect(const ray& r, double& tMin, double& tMax) const;

	vec3f GenerateRandomPoint() const
	{
		// TODO
	}

	// This method makes this boundingbox bigger.
	void mergeBoundingBox(const BoundingBox& other)
	{
		this->max = maximum(this->max, other.max);
		this->min = maximum(this->min, other.min);
	}
};

#endif //__BOUNDINGBOX_H__