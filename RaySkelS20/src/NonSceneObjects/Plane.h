#pragma once

#include "../scene/scene.h"

class Plane : public NoneSceneObject
{
private:
	vec3f n;	// Plane normal. Points x on the plane satisfy Dot(n, x) = d
	float d;	// d = dot(n, p) for a given point p on the plane

public:
	Plane() = delete;

	Plane(Scene* scene, const vec3f& n, float d)
		: NoneSceneObject(scene), n(n), d(d) {}

	Plane(Scene* scene, const vec3f& a, const vec3f& b, const vec3f& c)
		: NoneSceneObject(scene)
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

	static Plane GenerateRandomPlane(Scene* scene)
	{
		float x, y, z;
		x = (rand() % 100 - 50) / 100.0f;
		y = (rand() % 100 - 50) / 100.0f;
		z = (rand() % 100 - 50) / 100.0f;
		return Plane(scene, vec3f(x, y, z).normalize(), 0.0f);
	}
};