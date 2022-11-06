#include <cmath>
#include <iostream>

#include "light.h"
#include "ray.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P);
	vec3f point = P + d * RAY_EPSILON;
	ray r(point, d);
	isect i;
	// vec3f attn(1, 1, 1);
	if (getScene()->intersect(r, i))
	{
		// return vec3f(1, 1, 1);
		return i.getMaterial().kt;
	}
	else return vec3f(1, 1, 1);
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	// cout << P << endl;
	const double d = (P - position).length();
	double numerator =  a + b * d + c * d * d;
	if (numerator == 0) return 1.0;
	else if ( 1 / numerator < 1.0) return 1 / numerator;
	else {
		/*
		cout << "a: " << a << endl;
		cout << "b: " << b << endl;
		cout << "c: " << c << endl;
		cout << "d: " << d << endl;
		cout << "atten: " << 1 / numerator << endl;
		cout << endl; */
		return 1.0;
		
	}

}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P);
	vec3f point = P + d * RAY_EPSILON; 
	ray r(point, d);
	isect i;
	double tlight = (position - P).length();
	// vec3f attn(1, 1, 1);
	if (getScene()->intersect(r, i) && tlight > i.t)
	{
		// return vec3f(1, 1, 1);
		return i.getMaterial().kt;
	}
	else return vec3f(1, 1, 1);
}

/*
void PointLight::updateAttenuationCoeff(double& cAtt, double& lAtt, double& qAtt)
{
	a = cAtt;
	b = lAtt;
	c = qAtt;
}
*/
