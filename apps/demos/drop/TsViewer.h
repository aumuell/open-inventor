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
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes	: TsViewer
 |
 |   Author(s)	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SIMPLE_VIEWER_
#define  _SIMPLE_VIEWER_

#include <Inventor/Xt/viewers/SoXtViewer.h>
#include <Inventor/SbLinear.h>


//
// Class: TsViewer
//
// Drop Viewer - viewer which only operates in viewing mode.  Used with
// the drop game.
//
class TsViewer : public SoXtViewer {
 public:
    // constructor/destructor
    TsViewer(
        Widget parent = NULL,
        const char *name = NULL);
    ~TsViewer();
    
 protected:
    // redefine this to process the events
    virtual void	processEvent(XAnyEvent *anyevent);
    
    // redefine this to cache the renderArea size
    virtual void	sizeChanged(const SbVec2s &newSize);
    
 private:
    // viewer state variables
    int		    mode;
    SbBool	    createdCursors;
    Cursor	    vwrCursor, seekCursor;
    SbVec2s	    locator;    // mouse position
    SbVec2s	    movement;   // mouse movement
    SbVec2s	    windowSize; // cached size of the window
    
    // camera translation vars
    SbVec3f	    locator3D;
    SbPlane	    focalplane;
    float	    transXspeed, transYspeed;
    
    void	    switchMode(int newMode);
    void	    defineCursors();
    void	    translateCamera();
    void            rotateCamera();
    void	    computeTranslateValues();
    static void	    transWheelStartCB(void *v, float);
};

#endif  /* _TS_VIEWER_ */
