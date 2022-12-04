#include <cmath>

#include "light.h"
#include "ray.h"
#include "material.h"

#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

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

bool DirectionalLight::availableForLighting(const vec3f& i) const
{
	return true;
}

double DirectionalLight::softEdge(const vec3f& l2i) const
{
	return 1.0;
}

vec3f CircularLight::getPosition() const
{
	return position;
}

/// <summary>
/// 
/// </summary>
/// <param name="i">light to isectP</param>
/// <returns></returns>
bool CircularLight::availableForLighting(const vec3f& isectP) const
{
	vec3f l2i = isectP - getPosition();
	double area = abs(l2i.cross(orientation).length());
	double distance = area / orientation.length();
	return distance < radius;
}

double CircularLight::softEdge(const vec3f& isectP) const
{
	vec3f l2i = isectP - getPosition();
	double area = abs(l2i.cross(orientation).length());
	double distance = area / orientation.length();

	// Smoothstep: https://en.wikipedia.org/wiki/Smoothstep
	if (distance < radius * 0.8)
		return 1.0;
	else if (distance > radius)
		return 0.0;
	else
	{
		double x = (radius - distance) / 0.2 * radius;
		return 3 * x * x - 2 * x * x * x;
	}
}

bool TrianglarLight::availableForLighting(const vec3f& isectP) const
{
	// TODO: need debug
	vec3f dir = orientation.normalize();

	

	return true;
}

double TrianglarLight::softEdge(const vec3f& i) const
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
bool SpotLight::availableForLighting(const vec3f& isectP) const
{
	vec3f l2i = isectP - getPosition();
	double cosine = l2i.normalize().dot(orientation);
	return cosine > cosouter;
}

double SpotLight::softEdge(const vec3f& isectP) const
{
	vec3f l2i = isectP - getPosition();
	double cosine = l2i.normalize().dot(orientation.normalize());
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
