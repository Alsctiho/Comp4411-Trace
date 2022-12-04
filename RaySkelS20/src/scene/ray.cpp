#include "ray.h"
#include "material.h"

ray::ray() 
    : ray(vec3f(), vec3f()) {}

ray::ray(const vec3f& pp, const vec3f& dd)
    : p(pp), d(dd) {}

ray::ray(const ray& other)
    : p(other.p), d(other.d) {}

ray& ray::operator =(const ray& other)
{
    p = other.p; d = other.d; return *this;
}

vec3f ray::at(double t) const
{
    return p + (t * d);
}

vec3f ray::getPosition() const { return p; }
vec3f ray::getDirection() const { return d; }
vec3f ray::getIsecPosition(double t) const { return p + t * d; }

const Material& isect::getMaterial() const
{
    return material ? *material : obj->getMaterial();
}

isect& isect::operator =(const isect& other)
{
    if (this != &other)
    {
        obj = other.obj;
        t = other.t;
        N = other.N;
        //            material = other.material ? new Material( *(other.material) ) : 0;
        if (other.material)
        {
            if (material)
                *material = *other.material;
            else
                material = new Material(*other.material);
        }
        else
        {
            material = 0;
        }
    }
    return *this;
}
