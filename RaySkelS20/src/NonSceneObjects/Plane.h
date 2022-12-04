#pragma once

#include "hyperplane.h"
#include "..\scene\geometry.h"
#include "..\vecmath\vecmath.h"

class Geometry;

class Plane : public Hyperplane
{
private:
	vec3f n;	// Plane normal. Points x on the plane satisfy Dot(n, x) = d
	float d;	// d = dot(n, p) for a given point p on the plane

public:
	Plane() = delete;

	Plane(const vec3f& n, float d)
		: Hyperplane(), n(n.normalize()), d(d) {}

	Plane(const vec3f& n, const vec3f& p)
		: Hyperplane(), n(n.normalize())
	{
		d = n.dot(p);
	}

	Plane(const vec3f& a, const vec3f& b, const vec3f& c)
		: Hyperplane()
	{
		n = (b - a).cross(c - a).normalize();
		// n * (X - p) = 0
		// n * X - n * p = 0
		d = n.dot(a);	// define n * p = d
	}

	/// <summary>
	/// Determine whether a geometry is in the front/back/inside of the plane.
	/// </summary>
	/// <returns> return + is front, return 0 is intersct, return - is back </returns>
	int TestHalfSpace(Geometry* geometry);
	int TestHalfSpace(const vec3f& point);

	bool TestSameDirection(const vec3f& d) { return d.dot(n) > 0; }

	const vec3f& getNormal() { return n; }

	static vec3f GenerateRandomNormal()
	{
		float x, y, z;
		x = (rand() % 100 - 50) / 100.0f;
		y = (rand() % 100 - 50) / 100.0f;
		z = (rand() % 100 - 50) / 100.0f;
		return vec3f(x, y, z).normalize();
	}
};