#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	vec3f R0 = r.getPosition();
	vec3f Rd = r.getDirection();

	double tMin = -1.0e308; // 1.0e308 is close to infinity... close enough for us!
	double tMax = 1.0e308;

	double ttemp;
	int N_axis;
	vec3f N{ 0.0, 0.0, 0.0 };
	BoundingBox localBoundingBox = ComputeLocalBoundingBox();

	/*******
	if (localBoundingBox.intersect(r, tMin, tMax))
	{
		i.obj = this;
		i.t = tMin;
		i.N = r.at(tMin).normalize();
		return true;
	}
	else return false;
	************/
	
	for (int currentaxis = 0; currentaxis < 3; currentaxis++)
	{
		double vd = Rd[currentaxis];
		double v0 = R0[currentaxis];
		double vl = localBoundingBox.min[currentaxis];
		double vh = localBoundingBox.max[currentaxis];

		// if the ray is parallel to the face's plane (=0.0)
		if (vd == 0.0)
			//continue;
		{
			if (v0 < vl || v0 > vh) return false;
		}

		// double v1 = min[currentaxis] - R0[currentaxis];
		// double v2 = max[currentaxis] - R0[currentaxis];

		// two slab intersections
		double t1 = (vl - v0) / vd;
		double t2 = (vh - v0) / vd;

		if (t1 > t2) { // swap t1 & t2 since t1 intersection with near plane 
			ttemp = t1;
			t1 = t2;
			t2 = ttemp;
		}

		if (t1 > tMin)  // want largest tMin
		{
			tMin = t1;
			N_axis = currentaxis;
		}
		if (t2 < tMax)	// want smallest tMax
			tMax = t2;

		if (tMin > tMax) // box is missed
			return false;
		if (tMax < 0.0) // box is behind ray
			return false;

	}

	i.obj = this;
	i.t = tMin;
	// i.N = r.at(tMin).normalize();
	if (Rd[N_axis] > 0.0) N[N_axis] = -1.0;
	else N[N_axis] = 1.0;
	i.N = N;

	return true;
}

