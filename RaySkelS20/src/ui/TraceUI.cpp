//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

using std::endl;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_constAttSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nConstAtt = double(((Fl_Slider*)o)->value());
	((TraceUI*)(o->user_data()))->AttenCoeffChanged = true;
}

void TraceUI::cb_lineAttSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nLineAtt = double(((Fl_Slider*)o)->value());
	((TraceUI*)(o->user_data()))->AttenCoeffChanged = true;
}

void TraceUI::cb_quadAttSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nQuadAtt = double(((Fl_Slider*)o)->value());
	((TraceUI*)(o->user_data()))->AttenCoeffChanged = true;
}

void TraceUI::cb_threshSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThresh = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_antialiasingSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAntialiasing = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_testLightButton(Fl_Widget* o, void* v)
{
	if (int(((Fl_Button*)o)->value()))
		std::cout << 1 << endl;
	else std::cout << 0 << endl;
}

void TraceUI::cb_adaptiveSamplingButton(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAdaptiveSampling = int(((Fl_Button*)o)->value());
}

void TraceUI::cb_spatialStructure(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSpatialStructure = SpatialType(((Fl_Choice*)o)->value());
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getConstAtt()
{
	return m_nConstAtt;
}

double TraceUI::getLineAtt()
{
	return m_nLineAtt;
}

double TraceUI::getQuadAtt()
{
	return m_nQuadAtt;
}

double TraceUI::getThresh()
{
	return m_nThresh;
}

int TraceUI::getAntialiasing()
{
	return m_nAntialiasing;
}

bool TraceUI::AttenCoeffHasChanged()
{
	return AttenCoeffChanged;
}

bool TraceUI::isAdaptiveSampling()
{
	return m_nAdaptiveSampling;
}

SpatialType	TraceUI::getSpatialStructure()
{
	return m_nSpatialStructure;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nConstAtt = 0.25;
	m_nLineAtt = 0.25;
	m_nQuadAtt = 0.50;
	m_nAntialiasing = 0;
	m_nThresh = 0.00;

	m_mainWindow = new Fl_Window(100, 40, 350, 600, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 350, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install attenuation constant slider
		m_constAttSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_constAttSlider->user_data((void*)(this));
		m_constAttSlider->type(FL_HOR_NICE_SLIDER);
		m_constAttSlider->labelfont(FL_COURIER);
		m_constAttSlider->labelsize(12);
		m_constAttSlider->minimum(0.00);
		m_constAttSlider->maximum(1.00);
		m_constAttSlider->step(0.01);
		m_constAttSlider->value(m_nConstAtt);
		m_constAttSlider->align(FL_ALIGN_RIGHT);
		m_constAttSlider->callback(cb_constAttSlides);

		// install attenuation linear slider
		m_lineAttSlider = new Fl_Value_Slider(10, 105, 180, 20, "Attenuation, Linear");
		m_lineAttSlider->user_data((void*)(this));
		m_lineAttSlider->type(FL_HOR_NICE_SLIDER);
		m_lineAttSlider->labelfont(FL_COURIER);
		m_lineAttSlider->labelsize(12);
		m_lineAttSlider->minimum(0.00);
		m_lineAttSlider->maximum(1.00);
		m_lineAttSlider->step(0.01);
		m_lineAttSlider->value(m_nLineAtt);
		m_lineAttSlider->align(FL_ALIGN_RIGHT);
		m_lineAttSlider->callback(cb_lineAttSlides);

		// install attenuation quadric slider
		m_quadAttSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation, Quadric");
		m_quadAttSlider->user_data((void*)(this));
		m_quadAttSlider->type(FL_HOR_NICE_SLIDER);
		m_quadAttSlider->labelfont(FL_COURIER);
		m_quadAttSlider->labelsize(12);
		m_quadAttSlider->minimum(0.00);
		m_quadAttSlider->maximum(1.00);
		m_quadAttSlider->step(0.01);
		m_quadAttSlider->value(m_nQuadAtt);
		m_quadAttSlider->align(FL_ALIGN_RIGHT);
		m_quadAttSlider->callback(cb_quadAttSlides);

		// install thresh slider
		m_thresh = new Fl_Value_Slider(10, 155, 180, 20, "Threshold");
		m_thresh->user_data((void*)(this));
		m_thresh->type(FL_HOR_NICE_SLIDER);
		m_thresh->labelfont(FL_COURIER);
		m_thresh->labelsize(12);
		m_thresh->minimum(0.0);
		m_thresh->maximum(1.0);
		m_thresh->step(0.02);
		m_thresh->value(0.0);
		m_thresh->align(FL_ALIGN_RIGHT);
		m_thresh->callback(cb_threshSlides);

		// install antialiasing slider
		m_antialiasing = new Fl_Value_Slider(10, 180, 180, 20, "Antialiasing");
		m_antialiasing->user_data((void*)(this));
		m_antialiasing->type(FL_HOR_NICE_SLIDER);
		m_antialiasing->labelfont(FL_COURIER);
		m_antialiasing->labelsize(12);
		m_antialiasing->minimum(0);
		m_antialiasing->maximum(5);
		m_antialiasing->step(1);
		m_antialiasing->value(0.0);
		m_antialiasing->align(FL_ALIGN_RIGHT);
		m_antialiasing->callback(cb_antialiasingSlides);

		// install a light button
		m_Testing = new Fl_Light_Button(10, 205, 60, 25, "Test");
		m_Testing->user_data((void*)(this));
		m_Testing->callback(cb_testLightButton);

		// install a adaptive sampling button
		m_adaptiveSampling = new Fl_Light_Button(90, 205, 100, 25, "Ad Sampling");
		m_adaptiveSampling->user_data((void*)(this));
		m_adaptiveSampling->callback(cb_adaptiveSamplingButton);

		// install a spatial data structure algorithm
		m_spatialStructure = new Fl_Choice(120, 235, 100, 25, "Spatial Structure");
		m_spatialStructure->user_data((void*)(this));
		m_spatialStructure->add("Default");
		m_spatialStructure->add("BSP");
		m_spatialStructure->add("Octree");
		m_spatialStructure->callback(cb_spatialStructure);

		// install render button
		m_renderButton = new Fl_Button(270, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);
		
		// install stop button
		m_stopButton = new Fl_Button(270, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}