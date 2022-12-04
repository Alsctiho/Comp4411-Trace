//
// ray.h
//
// The low-level classes used by ray tracing: the ray and isect classes.
//

#ifndef __RAY_H__
#define __RAY_H__

#include "../vecmath/vecmath.h"

class SceneObject;
class Material;

// A ray has a position where the ray starts, and a direction (which should
// always be normalized!)

class ray {
public:
    ray();

    /// <summary>
    /// construct a ray by its position and direction.
    /// </summary>
    /// <param name="pp">position</param>
    /// <param name="dd">direction</param>
    ray(const vec3f& pp, const vec3f& dd);
    ray(const ray& other);
    ~ray() {}

    ray& operator =(const ray& other);

    vec3f at(double t) const;

    vec3f getPosition() const;
    vec3f getDirection() const;
    vec3f getIsecPosition(double t) const;

protected:
	vec3f p;
	vec3f d;
};

// The description of an intersection point.

class isect
{
public:
    isect()
        : obj( NULL ), t( 0.0 ), N(), material(0) {}

    ~isect()
    {
        delete material;
    }
    
    void setObject( SceneObject *o ) { obj = o; }
    void setT( double tt ) { t = tt; }
    void setN( const vec3f& n ) { N = n; }
    void setMaterial( Material *m ) { delete material; material = m; }
        
    isect& operator =(const isect& other);

public:
    const SceneObject 	*obj;
    double t;
    vec3f N;
    Material *material;         // if this intersection has its own material
                                // (as opposed to one in its associated object)
                                // as in the case where the material was interpolated

    const Material &getMaterial() const;
    // Other info here.
};

const double RAY_EPSILON = 0.00001;
const double NORMAL_EPSILON = 0.00001;

#endif // __RAY_H__
