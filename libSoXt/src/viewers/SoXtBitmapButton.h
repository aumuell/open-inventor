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
 * Copyright (C) 1992   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	SoXtBitmapButton - bitmap button convenience wrapper for the viewers
 |
 |   Description:
 |	A Motif push button gadget with a bitmap displayed on it.
 |	No border for the button is shown and the button can be selectable.
 |
 |   Author(s): Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_PIXMAP_BUTTON_
#define _SO_PIXMAP_BUTTON_

#include <X11/Intrinsic.h>
#include <Inventor/SbBasic.h>

class SoXtBitmapButton {
  public:
    SoXtBitmapButton(Widget parent, SbBool selectable);
    ~SoXtBitmapButton();
    
    // return the motif push button
    Widget	getWidget()	    { return widget; }
    
    // set the icon to use for the pixmap
    void	setIcon(char *icon, int width, int height);
    
    // Highlight the pixmap to show it it selected (must pass TRUE
    // to the constructor, in which case another pixmap with a highlight
    // color will be created for the button).
    void	select(SbBool onOrOff);
    SbBool	isSelected()	    { return selectFlag; }
    
  private:
    Widget	widget;
    SbBool	selectFlag, selectable;
    Pixmap	normalPixmap, selectPixmap;
};

#endif // _SO_PIXMAP_BUTTON_
