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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	_SoXtColorPatch
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#if DEBUG
#include <stream.h>
#endif

#include "_SoXtUIRegion.h"
#include "_SoXtColorPatch.h"

#include <GL/gl.h>


/*
 * Defines
 */

#define SIDE	(UI_THICK + 2 + UI_THICK)

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
_SoXtColorPatch::_SoXtColorPatch(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB, 
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
_SoXtColorPatch::_SoXtColorPatch(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool buildNow)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB, 
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
_SoXtColorPatch::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    // init local vars
    color[0] = color[1] = color[2] = 0;
    setGlxSize( SbVec2s(40, 40) );  // default size
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Dummy virtual destructor.
//
_SoXtColorPatch::~_SoXtColorPatch()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
//  Routine which draws the color patch
//
// Use: virtual public

void
_SoXtColorPatch::redraw()
//
////////////////////////////////////////////////////////////////////////
{
    if (! isVisible())
	return;
    
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    
    // draw border
    SbVec2s size = getGlxSize();
    SoDrawDownUIRegion(0, 0, size[0]-1, size[1]-1);
    
    // draw the patch color
    glColor3fv(color.getValue());
    glRecti(SIDE, SIDE, size[0] - SIDE, size[1] - SIDE);
    
    if (isDoubleBuffer())
	glXSwapBuffers(getDisplay(), getNormalWindow());
    else
	glFlush();
}


////////////////////////////////////////////////////////////////////////
//
//  Sets the patch color
//
// Use: public

void
_SoXtColorPatch::setColor(const SbColor &rgb)
//
////////////////////////////////////////////////////////////////////////
{
    // save color
    color = rgb;
    
    // now show the color change
    if (! isVisible())
	return;
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    
    glColor3fv(color.getValue());
    SbVec2s size = getGlxSize();
    glRecti(SIDE, SIDE, size[0] - SIDE, size[1] - SIDE);
    
    if (isDoubleBuffer())
	glXSwapBuffers(getDisplay(), getNormalWindow());
    else
	glFlush();
}

////////////////////////////////////////////////////////////////////////
//
//	This routine is called when the window size has changed.
//
// Use: virtual private

void
_SoXtColorPatch::sizeChanged(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    // reset projection
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    glViewport(0, 0, newSize[0], newSize[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, newSize[0], 0, newSize[1], -1, 1);
}
