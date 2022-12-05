//
// scene.h
//
// The Scene class and the geometric types that it can contain.
//

#ifndef __SCENE_H__
#define __SCENE_H__

#include <list>
#include <algorithm>
#include <memory>
 
#include "camera.h"
#include "spatialnode.h"
#include "geometry.h"
#include "light.h"
#include "../vecmath/vecmath.h"

using std::list;
using std::shared_ptr;

class BoundingBox;
class SceneElement;

// A SceneObject is a real actual thing that we want to model in the 
// world.  It has extent (its Geometry heritage) and surface properties
// (its material binding).  The decision of how to store that material
// is left up to the subclass.
class SceneObject
	: public Geometry
{
public:
	virtual const Material& getMaterial() const = 0;
	virtual void setMaterial( Material *m ) = 0;

protected:
	SceneObject( Scene *scene )
		: Geometry( scene ) {}
};



// A simple extension of SceneObject that adds an instance of Material
// for simple material bindings.
class MaterialSceneObject
	: public SceneObject
{ 
public:
	virtual ~MaterialSceneObject() { if( material ) delete material; }

	virtual const Material& getMaterial() const { return *material; }
	virtual void setMaterial( Material *m )	{ material = m; }

protected:
	MaterialSceneObject( Scene *scene, Material *mat ) 
		: SceneObject( scene ), material( mat ) {}
    //	MaterialSceneObject( Scene *scene ) 
	//	: SceneObject( scene ), material( new Material ) {}

	Material *material;
};


class Scene
{
public:
	typedef list<Light*>::iterator 			liter;
	typedef list<Light*>::const_iterator 	cliter;

	typedef list<Geometry*>::iterator 		giter;
	typedef list<Geometry*>::const_iterator cgiter;

    TransformRoot transformRoot;

public:
	Scene() 
		: transformRoot(), objects(), lights(), ambientLight(nullptr) {}
	virtual ~Scene();

	void add(Geometry* obj);

	void add( Light* light )
	{ lights.push_back( light ); }

	void addAmbientLight(vec3f color);

	bool intersect( const ray& r, isect& i ) const;

	/// <summary>
	/// Set up sp
	/// </summary>
	void initScene();

	const BoundingBox& getSceneBoundingBox() const { return sceneBounds; }

	list<Light*>::const_iterator beginLights() const { return lights.begin(); }
	list<Light*>::const_iterator endLights() const { return lights.end(); }
        
	Camera*			getCamera() { return &camera; }
	vec3f			getAmbientColor();
	

private:
    list<Geometry*> objects;
	list<Geometry*> nonboundedobjects;
	list<Geometry*> boundedobjects;
    list<Light*> lights;
    Camera camera;
	AmbientLight* ambientLight;
	std::shared_ptr<BSPNode> bsproot;
	std::shared_ptr<OctNode> octroot;
	
	// Each object in the scene, provided that it has hasBoundingBoxCapability(),
	// must fall within this bounding box.  Objects that don't have hasBoundingBoxCapability()
	// are exempt from this requirement.
	BoundingBox sceneBounds;
};

#endif // __SCENE_H__
