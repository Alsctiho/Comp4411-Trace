// The main ray tracer.

#include <Fl/fl_ask.h>
#include <iostream>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, traceUI->getThresh(), traceUI->getDepth()).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) && depth >= 0 ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		
		vec3f phong = m.shade(scene, r, i);

		vec3f incident = r.getDirection().normalize();
		vec3f normal = i.N.normalize();
		vec3f isecPos = r.getIsecPosition(i.t);

		// vec3f kr = (m.kr == vec3f(0.0f, 0.0f, 0.0f)) ? m.ks : m.kr;
		vec3f reflection, refraction;

		if (phong < thresh)  return phong; // adaptive termination based on thresh

		normal = (isInsideGeometry(incident, normal)) ? -normal : normal;

		// Reflection
		vec3f reflectedDir = reflect(-incident, normal).normalize();
		ray reflectedRay{ isecPos + reflectedDir * RAY_EPSILON, reflectedDir };
		reflection = prod(m.kr, (traceRay(scene, reflectedRay, thresh, depth - 1)));

		if (m.index == 1.0 && m.kt == vec3f(0.0, 0.0, 0.0))
			return phong + reflection;
		
		// Refraction
		// Step 1: Find refractive direction
		vec3f refractiveDir;
		// Step 2: Internal refraction -> inside and outside.
		if (isInsideGeometry(incident, normal))
		{
			refractiveDir = refract(-incident, normal, m.index, 1.0).normalize();
		}
		else
		{
			refractiveDir = refract(-incident, normal, 1.0, m.index).normalize();
		}

		
		if (refractiveDir == vec3f(0.0, 0.0, 0.0)) // handles tir
			return phong + reflection;

		// Step 3: Ray
		ray refractiveRay{ isecPos + refractiveDir * RAY_EPSILON, refractiveDir };
		refraction = prod(m.kt, traceRay(scene, refractiveRay, thresh, depth - 1));

		return phong + reflection + refraction;

	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

vec3f RayTracer::insideTraceRay(
	Scene* scene, const ray& r, const vec3f& thresh, int depth,
	const isect& i, const Material& m,
	const vec3f& incident, const vec3f& normal, vec3f& result)
{
	return vec3f(0.0, 0.0, 0.0);
}

vec3f RayTracer::outsideTraceRay(
	Scene* scene, const ray& r, const vec3f& thresh, int depth, 
	const isect& i, const Material& m, 
	const vec3f& incident, const vec3f& normal, vec3f& result)
{
	return vec3f(0.0, 0.0, 0.0);
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}
