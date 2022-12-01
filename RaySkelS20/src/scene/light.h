#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
	virtual vec3f getPosition() const = 0;
	virtual bool availableForLighting( const vec3f& d ) const = 0;
	virtual double softEdge( const vec3f& d ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	virtual vec3f getPosition() const;
	virtual bool availableForLighting(const vec3f& i) const;
	virtual double softEdge(const vec3f& d) const;

protected:
	vec3f 		orientation;

};

class CircularLight : public DirectionalLight
{
public:
	CircularLight(Scene* scene, const vec3f& orien, const vec3f& color, const vec3f& pos)
		: DirectionalLight(scene, orien, color), position(pos), radius(0.0) {}

	virtual vec3f getPosition() const;
	virtual bool availableForLighting(const vec3f& i) const;
	virtual double softEdge(const vec3f& i) const;

	//flaps
	void setRadius(double r) { this->radius = r; }

protected:
	vec3f		position;
	double		radius;
};

class TrianglarLight : public DirectionalLight
{
public:
	TrianglarLight(Scene* scene, const vec3f& orien, const vec3f& color, 
		const vec3f& pos1, const vec3f& pos2, const vec3f& pos3)
		: DirectionalLight(scene, orien, color), p0(pos1), p1(pos2), p2(pos3) {}
	
	virtual bool availableForLighting(const vec3f& i) const;
	virtual double softEdge(const vec3f& i) const;

protected:
	vec3f		p0;
	vec3f		p1;
	vec3f		p2;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color)
		: Light( scene, color ), position( pos ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	virtual vec3f getPosition() const;
	virtual bool availableForLighting(const vec3f& d) const;
	virtual double softEdge(const vec3f& d) const;
	void setCAtt(double a) { this->a = a; }
	void setLAtt(double b) { this->b = b; }
	void setQAtt(double c) { this->c = c; }
	// void updateAttenuationCoeff( double& cAtt, double& lAtt, double& qAtt );

protected:
	vec3f		position;
	double		a = 0;
	double		b = 0;
	double		c = 0;

};

class SpotLight
	: public PointLight
{
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& color, const vec3f& orien, double outer, double inner)
		: PointLight(scene, pos, color), orientation(orien.normalize()), outer(outer), inner(inner)
	{
		cosouter = cos(radian(outer));
		cosinner = cos(radian(inner));
	}
	
	virtual bool availableForLighting(const vec3f& isectP) const;
	virtual double softEdge(const vec3f& d) const;
	double getOuterCutoff() { return outer; }
	double getInnerCutoff() { return inner; }
	void setOuterCutoff(double o) { outer = o; }
	void setInnerCutoff(double i) { inner = i; }

protected:
	vec3f		orientation;
	double		outer;
	double		inner; // inner < outer
	double		cosouter;
	double		cosinner;
};

class AmbientLight: SceneElement
{
private:
	vec3f		color;

public:
	AmbientLight(Scene* scene, const vec3f& color)
		: color(color), SceneElement(scene) {}

	void add(vec3f other) { color += other; color.clamp(); }

	vec3f getColor() { return color; }
};

#endif // __LIGHT_H__
