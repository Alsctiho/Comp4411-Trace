#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <stack>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, int x, int y, int buffer_width, int buffer_height);
	vec3f traceAdaptive( Scene *scene, double x, double y, double offsetX, double offsetY, int depth);
	vec3f traceRay( Scene *scene, const ray& r, int depth );

	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );
	bool adaptiveTerminate(const vec3f& col);

	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;
	std::stack<const Material*> materials;

	friend class Camera;
};

#endif // __RAYTRACER_H__
