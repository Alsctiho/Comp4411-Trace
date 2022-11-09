#include <cmath>

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

vec3f DirectionalLight::getPosition() const
{
	return vec3f();
}

/// <summary>
/// 
/// </summary>
/// <param name="i">light to isectP</param>
/// <returns></returns>
bool DirectionalLight::availableForLighting(const vec3f& i) const
{
	if (isFlapping == false)
		return true;

	double area = abs(i.cross(orientation).length());
	double distance = area / orientation.length();
	cout << distance << endl;
	return distance < radius;
}

double DirectionalLight::softEdge(const vec3f& d) const
{
	return 1.0;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double aa = traceUI->getConstAtt();
	double bb = traceUI->getLineAtt()/5;
	double cc = traceUI->getQuadAtt()/10;
	const double d = (P - position).length();
	double numerator = 1.0;
	if (traceUI->AttenCoeffHasChanged())
		numerator =  aa + bb * d + cc * d * d;
	else numerator = a + b * d + c * d * d;

	if (numerator == 0) 
		return 1.0;
	else if ( 1 / numerator < 1.0) 
		return 1 / numerator;
	else 
		return 1.0;
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

vec3f PointLight::getPosition() const
{
	return position;
}

bool PointLight::availableForLighting(const vec3f& d) const
{
	return true;
}

double PointLight::softEdge(const vec3f& d) const
{
	return 1.0;
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

/// <summary>
/// 
/// </summary>
/// <param name="d">direction from light to isec</param>
/// <returns></returns>
bool SpotLight::availableForLighting(const vec3f& d) const
{
	double cosine = d.normalize().dot(orientation);
	return cosine > cosouter;
}

double SpotLight::softEdge(const vec3f& d) const
{
	double cosine = d.dot(orientation.normalize());
	return (cosine - cosouter) / (cosinner - cosouter);
}

/*
void PointLight::updateAttenuationCoeff(double& cAtt, double& lAtt, double& qAtt)
{
	a = cAtt;
	b = lAtt;
	c = qAtt;
}
*/
