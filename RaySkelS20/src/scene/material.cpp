
#include "material.h"

#include <algorithm>

namespace 
{
	double clamp(double d)
	{
		d = std::max(d, 0.0);
		d = std::min(d, 1.0);
		return d;
	}
}

vec3f Material::preshade(Scene* scene, const ray& r, const isect& i) const
{
	return vec3f();
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	// How can I get the intersaction point? 
	// How can I get the light positions? ans: scene->beginLights() 

	vec3f Iphong = ke;
	vec3f isectP = r.at(i.t);
	vec3f N = i.N.normalize();

	// 6.5.3.1 ambient reflection
	vec3f Ia = scene->getAmbientColor();
	// Iphong += prod(Ia, ka);
	Iphong += prod(prod(Ia, ka), vec3f(1.0, 1.0, 1.0) - kt);

	for (auto cliter = scene->beginLights(); cliter != scene->endLights(); ++cliter)
	{
		vec3f temp;

		// Spot light && flaps
		if (!(*cliter)->availableForLighting(isectP))
			continue;

		double dAtt = (*cliter)->distanceAttenuation(isectP);
		vec3f sAtt = (*cliter)->shadowAttenuation(isectP);
		vec3f atten = dAtt * sAtt;
		// dAtt = (dAtt < 1) ? dAtt : 1;
		vec3f Il = (*cliter)->getColor(isectP); // Color doesn't depend on isectP

		// 6.5.3.2 diffuse reflection
		vec3f L = (*cliter)->getDirection(isectP).normalize();
		double NdL = N.dot(L);
		if (NdL > 0)
		{
			// Iphong += prod(NdL * prod(Il, kd), atten);
			temp += prod(prod(NdL * prod(Il, kd), atten), vec3f(1.0, 1.0, 1.0) - kt);
		}

		// 6.5.3.3 specular reflection
		vec3f R = reflect(L, N).normalize();
		vec3f V = -r.getDirection();
		// vec3f V = r.getDirection().normalize();
		double RdV = R.dot(V);
		if (RdV > 0)
		{
			temp += prod(pow(RdV, shininess * 128) * prod(Il, ks), atten);
		}
		// double specular = pow(coss, shininess*128);

		Iphong += temp * (*cliter)->softEdge(isectP);
	}
	return Iphong;
}
