/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

/*
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Classes:
 |	_SoXtColorWheel
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#if DEBUG
#include <stream.h>
#endif
#include <inttypes.h>
#include <Inventor/Xt/devices/SoXtMouse.h>
#include "_SoXtColorWheel.h"
#include "_SoXtUIRegion.h"

#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>

/*
 * Defines
 */

#define CIRCLE	    5
#define STRIPES	    32
#define SPACE	    3
#define MARKER_SLOT 3

#define myMapcolor(i, r, g, b)	\
    XColor col;	\
    col.red = r << 8; \
    col.green = g << 8;	\
    col.blue = b << 8;	\
    col.flags = DoRed | DoGreen | DoBlue; \
    col.pixel = i;    \
    XStoreColor(getDisplay(), overlayColorMap, &col);


////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
_SoXtColorWheel::_SoXtColorWheel(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB | SO_GLX_OVERLAY, 
	    FALSE) // tell GLWidget not to build just yet  
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component is what the app wants, so buildNow = TRUE
    constructorCommon(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
_SoXtColorWheel::_SoXtColorWheel(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool buildNow)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB | SO_GLX_OVERLAY, 
	    FALSE) // tell GLWidget not to build just yet  
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component may be what the app wants, 
    // or it may want a subclass of this component. Pass along buildNow
    // as it was passed to us.
    constructorCommon(buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
_SoXtColorWheel::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    mouse = new SoXtMouse(ButtonPressMask | ButtonMotionMask | ButtonReleaseMask);

    // init local vars
    WYSIWYGmode = FALSE;
    hsvColor[0] = hsvColor[1] = 0.0;
    hsvColor[2] = 1.0;
    setGlxSize( SbVec2s(100, 100) );  // default size

    // callback lists
    startCallbacks  	= new SoCallbackList;
    changedCallbacks 	= new SoCallbackList;
    finishCallbacks 	= new SoCallbackList;
    interactive = FALSE;

    // allocate memory for the geometry/colors
    int num = 1 + CIRCLE * (STRIPES + 1);
    geometry = (SbVec2f *)malloc(num * sizeof(SbVec2f));
    defaultColors = (SbColor *)malloc(num * sizeof(SbColor));
    colors = (SbColor *)malloc(num * sizeof(SbColor));
    
    // ??? reset the geometry to prevent the window manager under PI a362
    // ??? to be killed (redrawing occurs even when window hasn't been
    // ??? mapped yet so those values would be bogus)
    for (int i=0; i<num; i++)
	geometry[i].setValue(0,0);
    
    // make the full intensity colors
    makeWheelColors(defaultColors, 1.0);
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}


////////////////////////////////////////////////////////////////////////
//
//    Destructor.
//

_SoXtColorWheel::~_SoXtColorWheel()
//
////////////////////////////////////////////////////////////////////////
{
    if (geometry != NULL)
	free(geometry);
    if (defaultColors != NULL)
	free(defaultColors);
    if (colors != NULL)
	free(colors);
	
    delete startCallbacks;
    delete changedCallbacks;
    delete finishCallbacks;
    
    delete mouse;
}


////////////////////////////////////////////////////////////////////////
//
//	This routine redraws the normal plane color wheel region.
//
// Use: virtual public

void
_SoXtColorWheel::redraw()
//
////////////////////////////////////////////////////////////////////////
{
    if (! isVisible())
	return;
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    
    // make a complete drawing
    drawWheelSurrounding();
    drawWheelColors();
    
    if (isDoubleBuffer())
	glXSwapBuffers(getDisplay(), getNormalWindow());
    else
	glFlush();
}


////////////////////////////////////////////////////////////////////////
//
//	This routine redraws the color wheel overlay region (marker).
//
// Use: virtual public

void
_SoXtColorWheel::redrawOverlay()
//
////////////////////////////////////////////////////////////////////////
{
    drawWheelMarker();
}


////////////////////////////////////////////////////////////////////////
//
//	Process the passed X event.
//
// Use: virtual public

void
_SoXtColorWheel::processEvent(XAnyEvent *xe)
//
////////////////////////////////////////////////////////////////////////
{
    short x, y;
    XButtonEvent *be;
    XMotionEvent *me;
    SbVec2s size = getGlxSize();
    
    switch (xe->type) {
	case ButtonPress:
	    be = (XButtonEvent *) xe;
	    if (be->button == Button1) {
		
		// check if click is in color wheel
		x = short(be->x) - cx;
		y = short(size[1] - be->y) - cy;
		if ((x*x + y*y) < ((radius+5)*(radius+5))) {
		    startCallbacks->invokeCallbacks(hsvColor);
		    interactive = TRUE;
		    moveWheelMarker(x, y);
		}
	    }
	    break;
	    
	case ButtonRelease:
	    be = (XButtonEvent *) xe;
	    if (be->button == Button1 && interactive) {
		interactive = FALSE;
		finishCallbacks->invokeCallbacks(hsvColor);
	    }
	    break;
	    
	case MotionNotify:
	    me = (XMotionEvent *) xe;
	    if (me->state & Button1Mask)
		moveWheelMarker(short(me->x) - cx, short(size[1] - me->y) - cy);
	    break;
    }
}


////////////////////////////////////////////////////////////////////////
//
//	This routine sets the color wheel current color.
//
// usage: public

void
_SoXtColorWheel::setBaseColor(
	const float hsv[3])
//
////////////////////////////////////////////////////////////////////////
{
    // check for redraw needs first
    SbBool valueChanged = (hsvColor[2] != hsv[2]);
    SbBool redrawColors = (WYSIWYGmode && valueChanged);
    SbBool redrawMarker = (hsvColor[0]!=hsv[0] || hsvColor[1]!=hsv[1]);
    
    // assign new color
    hsvColor[0] = hsv[0];
    hsvColor[1] = hsv[1];
    hsvColor[2] = hsv[2];
    
    // do wheel colors redraw
    if (redrawColors) {
	makeWheelColors(colors, hsv[2]);
	
	if (! isVisible())
	    return;
    	glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
	drawWheelColors();
	if (isDoubleBuffer())
	    glXSwapBuffers(getDisplay(), getNormalWindow());
	else
	    glFlush();
	checkMarkerColor();
    }
    
    // do marker redraw
    if (redrawMarker)
	drawWheelMarker();
    
    if (redrawMarker || valueChanged)
	changedCallbacks->invokeCallbacks(hsvColor);
}


////////////////////////////////////////////////////////////////////////
//
//    This routine sets the WYSIWYG mode.
//
// usage: public

void
_SoXtColorWheel::setWYSIWYG(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (WYSIWYGmode == flag)
	return;
    
    WYSIWYGmode = flag;
    
    // build WYSIWYG colors
    if (WYSIWYGmode)
	makeWheelColors(colors, hsvColor[2]);
    
    // now check if a redraw is needed
    if (hsvColor[2] != 1.0) {
	
	if (! isVisible())
	    return;
    	glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
	drawWheelColors();
	if (isDoubleBuffer())
	    glXSwapBuffers(getDisplay(), getNormalWindow());
	else
	    glFlush();
	checkMarkerColor();
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  	This builds the parent Glx widget, then registers interest
// in mouse events.
//
// Use: protected
Widget
_SoXtColorWheel::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget mgr = SoXtGLWidget::buildWidget(parent);
    
    Widget w = getOverlayWidget() ? getOverlayWidget() : getNormalWidget();
    mouse->enable(w, 
		(XtEventHandler) SoXtGLWidget::eventHandler,
		(XtPointer) this);
    
    return mgr;
}


////////////////////////////////////////////////////////////////////////
//
//  This routine is called at window creation time to init the GL state
//
// Use: virtual protected

void
_SoXtColorWheel::initOverlayGraphic()
//
////////////////////////////////////////////////////////////////////////
{
    myMapcolor(MARKER_SLOT, 0, 0, 0);
    blackMarker = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
//	This routine is called when window changed size.
//
// Use: virtual private

void
_SoXtColorWheel::sizeChanged(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // recompute things that depend on window size
    //
    short min = (newSize[0] < newSize[1]) ? newSize[0] : newSize[1];
    radius = (min - (4*UI_THICK + 2*SPACE)) / 2;
    cx = newSize[0] / 2;
    cy = newSize[1] / 2;
    
    makeWheelGeometry();
    
    // reset projection
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    glViewport(0, 0, newSize[0], newSize[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, newSize[0], 0, newSize[1], -1, 1);
    
    if (getOverlayWindow() != 0) {
	glXMakeCurrent(getDisplay(), getOverlayWindow(), getOverlayContext());
	glViewport(0, 0, newSize[0], newSize[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, newSize[0], 0, newSize[1], -1, 1);
    }
}


////////////////////////////////////////////////////////////////////////
//
//	This routine computes the color wheel geometry.
//
// Use: private

void
_SoXtColorWheel::makeWheelGeometry()
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f *v = geometry;
    float c[STRIPES+1][2];
    int i, j;
    
    // makes the master circle
    float r = radius / float(CIRCLE);
    float angle = 2.0 * M_PI / float(STRIPES);
    c[0][0] = c[STRIPES][0] = r;
    c[0][1] = c[STRIPES][1] = 0.0;
    for (i=1; i<STRIPES; i++) {
	c[i][0] = r * cosf(i*angle);
	c[i][1] = r * sinf(i*angle);
    }
    
    //
    // now build the color wheel geometry
    //
    
    // center point
    (*v)[0] = cx;
    (*v++)[1] = cy;
    
    for (j=1; j<=CIRCLE; j++) {
	for (i=0; i<=STRIPES; i++) {
	    (*v)[0] = cx + j * c[i][0];
	    (*v++)[1] = cy + j * c[i][1];
	}
    }
}


////////////////////////////////////////////////////////////////////////
//
//	This routine computes the color wheel colors given a color 
//  intensity.
//
// Use: private

void
_SoXtColorWheel::makeWheelColors(
	SbColor *c, 
	float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    float hsv[3];
    float Hinc, Sinc;
    int i, j;
    
    hsv[2] = intensity;
    Hinc = 1.0 / float(STRIPES);
    Sinc = 1.0 / float(CIRCLE);
    
    // center color
    (*c)[0] = (*c)[1] = (*c)[2] = intensity; c++;
    
    //
    // now make the wheel colors
    //
    for (j=1; j<=CIRCLE; j++) {
	
	hsv[1] = j * Sinc;
	for (i=0; i<STRIPES; i++) {
	    
	    hsv[0] = i * Hinc;
	    (c++)->setHSVValue(hsv);
	}
	
	// last color is same as first color
	(*c)[0] = (*(c-STRIPES))[0];
	(*c)[1] = (*(c-STRIPES))[1];
	(*c)[2] = (*(c-STRIPES))[2];
	c++;
    }
}


////////////////////////////////////////////////////////////////////////
//
//	This routine draws the color wheel surroundings.
//
// Use: private

void
_SoXtColorWheel::drawWheelSurrounding()
//
////////////////////////////////////////////////////////////////////////
{
    glClearColor(.66, .66, .66, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw outer border
    SbVec2s size = getGlxSize();
    SoDrawDownUIBorders(0, 0, size[0]-1, size[1]-1);
}


////////////////////////////////////////////////////////////////////////
//
//	This routine draws the color wheel top part (colors).
//
// Use: private

void
_SoXtColorWheel::drawWheelColors()
//
////////////////////////////////////////////////////////////////////////
{
    SbColor *c1, *c2;
    SbVec2f *v1, *v2;
    int i, j;
    
    //
    // draw center circle as triangle fan
    //
    c1 = (WYSIWYGmode) ? colors : defaultColors;
    v1 = geometry;
    
    glBegin(GL_TRIANGLE_FAN);
    for (i=0; i<(STRIPES+2); i++) {
	glColor3fv((c1++)->getValue());
	glVertex2fv((v1++)->getValue());
    }
    glEnd();
    
    //
    // draw the remaining stripes as quad meshes
    //
    c1 = (WYSIWYGmode) ? colors : defaultColors;
    v1 = geometry;
    c1++; v1++;
    c2 = c1 + STRIPES + 1;
    v2 = v1 + STRIPES + 1;
    
    for (j=1; j<CIRCLE; j++) {
	
	glBegin(GL_QUAD_STRIP);
	for (i=0; i<=STRIPES; i++) {
	    glColor3fv((c1++)->getValue());
	    glVertex2fv((v1++)->getValue());
	    glColor3fv((c2++)->getValue());
	    glVertex2fv((v2++)->getValue());
	}
	glEnd();
    }
}


////////////////////////////////////////////////////////////////////////
//
//	This routine checks the marker color and changes it if needed.
//
// Use: private

void
_SoXtColorWheel::checkMarkerColor()
//
////////////////////////////////////////////////////////////////////////
{
    if (getOverlayWindow() == 0)
	return;
    
    // map a black or white color depending on background
    
    if (WYSIWYGmode && hsvColor[2]<0.6) {	// should be white
	if (blackMarker) {
	    glXMakeCurrent(getDisplay(), getOverlayWindow(), getOverlayContext());
	    myMapcolor(MARKER_SLOT, 255, 255, 255);
	    blackMarker = FALSE;
	}
    } 
    else {	// should be black
	if (!blackMarker) {
	    glXMakeCurrent(getDisplay(), getOverlayWindow(), getOverlayContext());
	    myMapcolor(MARKER_SLOT, 0, 0, 0);
	    blackMarker = TRUE;
	}
   }
}

////////////////////////////////////////////////////////////////////////
//
//	This routine draws the color marker feedback (in overlay).
//
// Use: private

void
_SoXtColorWheel::drawWheelMarker()
//
////////////////////////////////////////////////////////////////////////
{
    if (getOverlayWindow() == 0 || ! isVisible())
	return;
    glXMakeCurrent(getDisplay(), getOverlayWindow(), getOverlayContext());
    
    // find radius and angle from hsv color, from which position is determined.
    short x, y;
    float rad = hsvColor[1] * radius;
    float angle = 2.0 * M_PI * hsvColor[0];
    x = cx + short(rad * cosf(angle));
    y = cy + short(rad * sinf(angle));
    
    //
    // now draw dot in overlay plane
    //
    glClearIndex(0);
    glClear(GL_COLOR_BUFFER_BIT);
    glIndexi(MARKER_SLOT);
    
    // ??? doing a GL_LINE_LOOP seems to be missing the top right
    // ??? pixel due to subpixel == TRUE in openGL.
    glBegin(GL_LINE_STRIP);
    glVertex2s(x+3, y+3); glVertex2s(x+3, y-3);
    glVertex2s(x-3, y-3); glVertex2s(x-3, y+3);
    glVertex2s(x+3+1, y+3);
    glEnd();
    
    glFlush();
}

////////////////////////////////////////////////////////////////////////
//
//	This routine moves the color maker based of mouse position.
//
// Use: private
//
void
_SoXtColorWheel::moveWheelMarker(
	short x, short y)	    // wheel center relative coordinates
//
////////////////////////////////////////////////////////////////////////
{
    // find the saturation based on distance
    float s = sqrtf(x*x + y*y) / float(radius);
    if (s > 1.0)
	s = 1.0;
    
    // now find the hue based on the angle
    float angle = atan2(float(y), float(x));
    if (angle < 0.0)
	angle += 2.0 * M_PI;
    float h = angle / (2.0 * M_PI);
    
    // check if redraw and callback are needed
    if (hsvColor[0] != h || hsvColor[1] != s) {
	
	hsvColor[0] = h;
	hsvColor[1] = s;
	
	drawWheelMarker();
	changedCallbacks->invokeCallbacks(hsvColor);
    }
}
