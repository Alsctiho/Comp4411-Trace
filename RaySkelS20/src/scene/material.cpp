#include "ray.h"
#include "material.h"
#include "light.h"

namespace 
{
	double clamp(double d)
	{
		d = max(d, 0.0);
		d = min(d, 1.0);
		return d;
	}
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

	vec3f intensity;
	vec3f isectP = r.getIsecPosition(i.t);
	vec3f isectN = i.N.normalize();
	// 6.5.3.1 ambient reflection
	vec3f aCol = scene->getAmbientColor();
	intensity += aCol.hadamard(ka);

	for (auto cliter = scene->beginLights(); cliter != scene->endLights(); ++cliter)
	{
		vec3f lDir = (*cliter)->getDirection(isectP);
		double cosd = clamp((-lDir).dot(isectN));
		if (cosd > 0)
		{
			// 6.5.3.2 diffuse reflection
			vec3f lCol = (*cliter)->getColor(isectP); // Color doesn't depend on isectP
			double dAtt = (*cliter)->distanceAttenuation(isectP);
			intensity += dAtt * cosd * lCol.hadamard(kd);

			// 6.5.3.3 specular reflection
			vec3f rDir = lDir - 2 * lDir.dot(isectN) * isectN;
			vec3f cDir = -scene->getCamera()->getLook().normalize();
			double coss = clamp(cDir.dot(rDir));
			//double specular = pow(coss, shininess);
			intensity += dAtt * coss * lCol.hadamard(ks);
		}

	}

	return intensity;
}
