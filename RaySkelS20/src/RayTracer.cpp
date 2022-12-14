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
vec3f RayTracer::trace( Scene *scene, int i, int j, int buffer_width, int buffer_height)
{
	vec3f result;

	if (traceUI->getAntialiasing() == 0)
	{
		// x, y belongs to [0, 1)
		double x = double(i) / double(buffer_width);
		double y = double(j) / double(buffer_height);

		ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));

		scene->getCamera()->rayThrough(x, y, r);
		result = traceRay(scene, r, traceUI->getDepth()).clamp();
	}
	else
	{
		int range = traceUI->getAntialiasing();
		vector<ray> rays;
		scene->getCamera()->rayThroughAntialising(i, j, range, rays, buffer_width, buffer_height);

		for(ray r : rays)
			result += traceRay(scene, r, traceUI->getDepth()).clamp();

		result /= rays.size();
	}

	return result;
}

vec3f RayTracer::traceAdaptive(Scene* scene, double x, double y, double offsetX, double offsetY, int depth)
{
	vec3f result;

	if (depth == 0)
	{
		ray r;
		scene->getCamera()->rayThrough(x, y, r);
		result = traceRay(scene, r, traceUI->getDepth()).clamp();
	}
	else
	{
		double newOffsetX = offsetX / 2;
		double newOffsetY = offsetY / 2;

		ray ul, ur, dl, dr, c;
		scene->getCamera()->rayThrough(x, y, ul);
		scene->getCamera()->rayThrough(x + offsetX, y, ur);
		scene->getCamera()->rayThrough(x, y + offsetY, dl);
		scene->getCamera()->rayThrough(x + offsetX, y + offsetY, dr);
		scene->getCamera()->rayThrough(x + newOffsetX, y + newOffsetY, c);

		vec3f rul, rur, rdl, rdr, rc;
		rul = traceRay(scene, ul, traceUI->getDepth()).clamp();
		rur = traceRay(scene, ur, traceUI->getDepth()).clamp();
		rdl = traceRay(scene, dl, traceUI->getDepth()).clamp();
		rdr = traceRay(scene, dr, traceUI->getDepth()).clamp();
		rc = traceRay(scene, c, traceUI->getDepth()).clamp();
		
		if((rul - rc).length() > 0.05)
			rul = traceAdaptive(scene, x, y, newOffsetX, newOffsetY, depth - 1);
		if((rur - rc).length() > 0.05)
			rur = traceAdaptive(scene, x + offsetX, y, newOffsetX, newOffsetY, depth - 1);
		if((rdl - rc).length() > 0.05)
			rdl = traceAdaptive(scene, x, y + offsetY, newOffsetX, newOffsetY, depth - 1);
		if((rdr - rc).length() > 0.05)
			rdr = traceAdaptive(scene, x + offsetX, y + offsetY, newOffsetX, newOffsetY, depth - 1);

		result = (rul + rur + rdl + rdr + rc) / 5;
	}

	return result;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, int depth )
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

		vec3f normal = i.N.normalize();
		const Material& m = i.getMaterial();
		const vec3f& incident = r.getDirection().normalize();
		const vec3f& isecPos = r.getIsecPosition(i.t);
		vec3f phong, reflection, refraction;

		bool inside = isInsideGeometry(incident, normal);

		if (depth == traceUI->getDepth())
			phong += m.preshade(scene, r, i);

		phong += m.shade(scene, r, i);
		normal = (inside) ? -normal : normal;

		if (adaptiveTerminate(phong))
			return phong;

		// Reflection
		vec3f reflectedDir = reflect(-incident, normal).normalize();
		ray reflectedRay{ isecPos + reflectedDir * RAY_EPSILON, reflectedDir };
		reflection = prod(m.kr, (traceRay(scene, reflectedRay, depth - 1)));

		if (m.index == 1.0 && m.kt == vec3f(0.0, 0.0, 0.0))
			return phong + reflection;
		
		// Refraction
		// Step 1: Find refractive direction
		vec3f refractiveDir;
		// Step 2: Internal refraction -> inside and outside.
		if (inside)
		{
			//const Material* insideMaterial = materials.top();
			//materials.pop();
			//const Material* outsideMaterial = materials.top();
			//refractiveDir = refract(-incident, normal, insideMaterial->index, outsideMaterial->index);
			refractiveDir = refract(-incident, normal, m.index, 1.0).normalize();
		}
		else
		{
			// should be vacuum for the first recursion.
			//const Material* outsideMaterial = materials.top();
			//const Material* insideMaterial = &m;
			//materials.push(insideMaterial);
			//refractiveDir = refract(-incident, normal, outsideMaterial->index, insideMaterial->index);
			refractiveDir = refract(-incident, normal, 1.0, m.index).normalize();
		}
		
		if (refractiveDir == vec3f(0.0, 0.0, 0.0)) // handles tir
			return phong + reflection;

		// Step 3: Ray
		ray refractiveRay{ isecPos + refractiveDir * RAY_EPSILON, refractiveDir };
		refraction = prod(m.kt, traceRay(scene, refractiveRay, depth - 1));

		return phong + reflection + refraction;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;

	// The first material is vacuum
	materials.push(new Material());
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;

	delete materials.top();
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


	// default sampling and adaptive sampling
	if (traceUI->isAdaptiveSampling())
	{
		double x = (double)i / (double)buffer_width;
		double y = (double)j / (double)buffer_height;

		double offsetX = 1.0 / (double)buffer_width;
		double offsetY = 1.0 / (double)buffer_height;

		col = traceAdaptive(scene, x, y, offsetX, offsetY, 2);
	}
	else
	{
		col = trace(scene, i, j, buffer_width, buffer_height);
	}


	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

bool RayTracer::adaptiveTerminate(const vec3f& col)
{
	return col.length() < traceUI->getThresh();
}
