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
 |	This class provides an easy to use interface to get X resource
 |   values for a widget. Special care is taken for SoComponents. 
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XTRESOURCE_
#define _SO_XTRESOURCE_

#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbColor.h>

class SoXtResource {
  public:
    // pass in the widget for which X resource values are desired
    SoXtResource(Widget w);
    ~SoXtResource();
    
    //
    // These methods look up X resource values by calling XrmQGetResource.
    // They return TRUE if successfully located the resource.
    //
    // NOTE: you do not have to specify the complete widget hierarchy.
    // This is automatically performed by this class. Simply name the
    // specific resource that should be searched for. The most efficient
    // use of this class is to construct an instance for a widget, then 
    // call getResource repeatedly with different resource strings which
    // are valid for the widget.
    //
    // For example:
    //	  SoXtResource xr(colorEditor->getWidget());
    //	  xr.getResource("wysiwyg", "Wysiwyg", onOrOff);
    //	  xr.getResource("colorSliders", "ColorSliders", rgbOrHsv);
    //	  xr.getResource("updateFrequency", "UpdateFrequency", freq);
    
    // C-api: name=getCol
    SbBool   	getResource(char *resName, char *resClass, SbColor &c);
    // C-api: name=getInt
    SbBool   	getResource(char *resName, char *resClass, short &i);
    // C-api: name=getUShort
    SbBool   	getResource(char *resName, char *resClass, unsigned short &u);
    // C-api: name=getStr
    SbBool   	getResource(char *resName, char *resClass, char *&s);
    // C-api: name=getBool
    SbBool   	getResource(char *resName, char *resClass, SbBool &b);
    // C-api: name=getFloat
    SbBool   	getResource(char *resName, char *resClass, float &f);

  private:
    XrmQuarkList    nameList;
    XrmQuarkList    classList;
    int	    	    listSize;
    Display	    *display;

    // These methods look up X resource values by calling XrmGetResource.
    // e.g. getResource("*backgroundColor", "*BackgroundColor", c);
    // They return TRUE if successfully located the resource.
    
    static SbBool getResource(Display *, char *sName, char *sClass, SbColor &c);
    static SbBool getResource(Display *, char *sName, char *sClass, short &i);
    static SbBool getResource(Display *, char *sName, char *sClass, unsigned short &u);
    static SbBool getResource(Display *, char *sName, char *sClass, char *&s);
    static SbBool getResource(Display *, char *sName, char *sClass, SbBool &b);
    static SbBool getResource(Display *, char *sName, char *sClass, float &f);


    // These methods look up X resource values by calling XrmQGetResource.
    // That is, they use XrmQuarks instead of strings. (A quark is an Xrm
    // notion - it is simply a key into a hash table of strings.)
    // They are used by SoXtResource for SoComponents.
    // They return TRUE if successfully located the resource.
    
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, SbColor &c);
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, short &i);
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, unsigned short &u);
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, char *&s);
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, SbBool &b);
    static SbBool getResource(Display *, XrmQuarkList qName, XrmQuarkList qClass, float &f);
};

#endif /* _SO_XTRESOURCE_ */
