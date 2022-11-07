#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );
	vec3f insideTraceRay(
		Scene* scene, const ray& r, const vec3f& thresh, int depth,
		const isect& i, const Material& m,
		const vec3f& incident, const vec3f& normal, vec3f& result);
	vec3f outsideTraceRay(
		Scene* scene, const ray& r, const vec3f& thresh, int depth,
		const isect& i, const Material& m,
		const vec3f& incident, const vec3f& normal, vec3f& result);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
