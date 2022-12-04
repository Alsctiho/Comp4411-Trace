#ifndef __TRANSFORMNODE_H__
#define __TRANSFORMNODE_H__

#include <list>
#include "..\vecmath\vecmath.h"

using std::list;

class TransformNode
{
protected:

    // information about this node's transformation
    mat4f    xform;
    mat4f    inverse;
    mat3f    normi;

    // information about parent & children
    TransformNode* parent;
    list<TransformNode*> children;

public:
    typedef list<TransformNode*>::iterator          child_iter;
    typedef list<TransformNode*>::const_iterator    child_citer;

    ~TransformNode()
    {
        for (child_iter c = children.begin(); c != children.end(); ++c)
            delete (*c);
    }

    TransformNode* createChild(const mat4f& xform)
    {
        TransformNode* child = new TransformNode(this, xform);
        children.push_back(child);
        return child;
    }

    // Coordinate-Space transformation
    vec3f globalToLocalCoords(const vec3f& v)
    {
        return inverse * v;
    }

    vec3f localToGlobalCoords(const vec3f& v)
    {
        return xform * v;
    }

    vec4f localToGlobalCoords(const vec4f& v)
    {
        return xform * v;
    }

    vec3f localToGlobalCoordsNormal(const vec3f& v)
    {
        return (normi * v).normalize();
    }

protected:
    // protected so that users can't directly construct one of these...
    // force them to use the createChild() method.  Note that they CAN
    // directly create a TransformRoot object.
    TransformNode(TransformNode* parent, const mat4f& xform)
        : children()
    {
        this->parent = parent;
        if (parent == NULL)
            this->xform = xform;
        else
            this->xform = parent->xform * xform;

        inverse = this->xform.inverse();
        normi = this->xform.upper33().inverse().transpose();
    }
};

class TransformRoot : public TransformNode
{
public:
    TransformRoot()
        : TransformNode(NULL, mat4f()) {}
};

#endif //__TRANSFORMNODE_H__