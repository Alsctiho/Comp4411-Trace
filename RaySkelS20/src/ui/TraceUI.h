//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

enum class SpatialType
{
	Default,
	BSP,
	Octree,
};

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_constAttSlider;
	Fl_Slider*			m_lineAttSlider;
	Fl_Slider*			m_quadAttSlider;
	Fl_Slider*			m_thresh;
	Fl_Slider*			m_antialiasing;

	Fl_Light_Button*	m_Testing;
	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;
	Fl_Light_Button*	m_adaptiveSampling;
	Fl_Choice*			m_spatialStructure;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double		getConstAtt();
	double		getLineAtt();
	double		getQuadAtt();
	bool		AttenCoeffHasChanged();
	bool		isAdaptiveSampling();
	SpatialType	getSpatialStructure();

	double		getThresh();
	int			getAntialiasing();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nConstAtt;
	double		m_nLineAtt;
	double		m_nQuadAtt;
	double		m_nThresh;
	int			m_nAntialiasing;
	bool		AttenCoeffChanged = false;
	bool		m_nAdaptiveSampling = false;
	SpatialType	m_nSpatialStructure;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_constAttSlides(Fl_Widget* o, void* v);
	static void cb_lineAttSlides(Fl_Widget* o, void* v);
	static void cb_quadAttSlides(Fl_Widget* o, void* v);
	static void cb_threshSlides(Fl_Widget* o, void* v);
	static void cb_antialiasingSlides(Fl_Widget* o, void* v);
	static void cb_spatialStructure(Fl_Widget* o, void* v);

	static void cb_testLightButton(Fl_Widget* o, void* v);
	static void cb_adaptiveSamplingButton(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
