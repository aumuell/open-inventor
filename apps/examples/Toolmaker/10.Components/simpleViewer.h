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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes	: simpleViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SIMPLE_VIEWER_
#define  _SIMPLE_VIEWER_

#include <Inventor/Xt/viewers/SoXtFullViewer.h>
#include <Inventor/SbLinear.h>


//////////////////////////////////////////////////////////////////////////////
//
//  Class: simpleViewer
//
//	simpleViewer - viewer which translates the camera 
//  in the viewer plane.
//
//////////////////////////////////////////////////////////////////////////////

class simpleViewer : public SoXtFullViewer {
 public:
    // constructor/destructor
    simpleViewer(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SoXtFullViewer::BuildFlag flag = BUILD_ALL, 
	SoXtViewer::Type type = BROWSER);
    ~simpleViewer();
    
    // redefine this to also change the cursor (viewerCursor)
    virtual void	setViewing(SbBool onOrOff);
    
 protected:
    // redefine this to process the events
    virtual void	processEvent(XAnyEvent *anyevent);
    
    // redefine this to also change the cursor (seekCursor)
    virtual void	setSeekMode(SbBool onOrOff);
    
    // Define those thumb wheels to translate in the viewer plane
    virtual void    	bottomWheelMotion(float newVal);
    virtual void    	leftWheelMotion(float newVal);
    virtual void    	rightWheelMotion(float newVal);
    virtual void    	bottomWheelStart();
    virtual void    	leftWheelStart();
    
    // redefine this to customize the preference sheet
    virtual void	createPrefSheet();
    
    // Define this to bring the viewer help card
    virtual void	openViewerHelpCard();
    
 private:
    // viewer state variables
    int		    mode;
    SbBool	    createdCursors;
    Cursor	    vwrCursor, seekCursor;
    SbVec2s	    locator; // mouse position
    
    // camera translation vars
    SbVec3f	    locator3D;
    SbPlane	    focalplane;
    float	    transXspeed, transYspeed;
    
    void	    switchMode(int newMode);
    void	    defineCursors();
    void	    translateCamera();
    void	    computeTranslateValues();
};

#endif  /* _SIMPLE_VIEWER_ */
