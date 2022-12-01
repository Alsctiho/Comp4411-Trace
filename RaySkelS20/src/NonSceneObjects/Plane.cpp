#include "Plane.h"

int Plane::TestHalfSpace(Geometry* object)
{
	const BoundingBox& bounds = object->getBoundingBox();

	vec3f min = bounds.min;
	vec3f max = bounds.max;

	int r, t;
	r = TestHalfSpace(vec3f(min[0], min[1], min[2]));
	if (r == 0) return 0;
	t = TestHalfSpace(vec3f(max[0], min[1], min[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(min[0], max[1], min[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(max[0], max[1], min[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(min[0], min[1], max[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(max[0], min[1], max[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(min[0], max[1], max[2]));
	if (r * t < 0) return 0;
	t = TestHalfSpace(vec3f(max[0], max[1], max[2]));
	if (r * t < 0) return 0;
	
	return r;
}

int Plane::TestHalfSpace(const vec3f& p)
{
	return n.dot(p) + d;
}