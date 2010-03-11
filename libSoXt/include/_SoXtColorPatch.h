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
 |   Description:
 |	This file contains the class description for the _SoXtColorPatch.
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_COLOR_PATCH_
#define  _SO_XT_COLOR_PATCH_

#include <Inventor/Xt/SoXtGLWidget.h>
#include <Inventor/SbColor.h>


//////////////////////////////////////////////////////////////////////////////
//
//  Class: _SoXtColorPatch
//
//	This class simply draws a 3D looking patch of color.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtColPatch
class _SoXtColorPatch : public SoXtGLWidget {

  public:
    
    _SoXtColorPatch(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~_SoXtColorPatch();
    
    //
    // set/get routines to specify the patch top color
    //
    // C-api: name=setCol
    void		setColor(const SbColor &rgb);
    // C-api: name=getCol
    const SbColor &	getColor()	{ return color; }
  
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtSlider::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    _SoXtColorPatch(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);
    
  private:
    
    // redefine to do ColorPatch specific things
    virtual void    redraw();
    virtual void    sizeChanged(const SbVec2s &newSize);
    
    // local variables
    SbColor	color;

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};


#endif  // _SO_XT_COLOR_PATCH_
