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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoMaterialBundle class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MATERIAL_BUNDLE
#define  _SO_MATERIAL_BUNDLE

#include <Inventor/bundles/SoBundle.h>

class SoGLLazyElement;


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMaterialBundle
//
//  Bundle that allows shapes to deal with materials more easily.
//  Since materials deal with several elements simultaneously, this
//  bundle hides a lot of this work from shapes.
//
//  Much of the functionality of this bundle has been replaced by the
//  SoLazyElement. 
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoMaterialBundle : public SoBundle {

  public:
    // Constructor - takes the action the bundle is used for
    SoMaterialBundle(SoAction *action);

    // Destructor
    ~SoMaterialBundle();

    // This can be called by shapes to prepare for sending multiple
    // materials later on. It is not required, since all of the send
    // methods call it if necessary. However, it allows the bundle to
    // set up some optimizations that are not possible to do later on.
    // This should never be called between calls to glBegin() and
    // glEnd().
    void		setUpMultiple()	   
	    { accessElements(FALSE, FALSE);
	      sendMultiple = TRUE; }

    // Makes sure the first defined material in the state is sent to
    // GL. This should never be called between calls to glBegin() and
    // glEnd().
    void		sendFirst()		{ send(0, FALSE); }

    // Sends indexed material to the GL if it's not already there.
    // The second paramater should be TRUE if this send is between a
    // glBegin() and glEnd() call, in which case the stipple pattern
    // commands for screen-door transparency won't be sent.
    void		send(int index, SbBool isBetweenBeginEnd)
	{  reallySend(index, isBetweenBeginEnd, FALSE);  }
	    
    // Sends indexed material to the GL even if it's already there.
    // This method can be used by SoMaterial and related nodes to send
    // the base material to the GL to avoid cache dependencies on materials.
    void		forceSend(int index)
	{ reallySend(index, FALSE, TRUE); }

    // Returns TRUE if only base color part of material is used
    SbBool		isColorOnly() const	{ return colorOnly; }

  private:
    SbBool		firstTime;	// TRUE if nothing sent yet
    int			lastIndex;	// Last index reallySend()ed
    SbBool		colorOnly;	// TRUE if only base color sent
    SbBool		fastColor;	// TRUE if can send color for diffColor
    int			numMaterials;	// Number of materials 
    SbBool		sendMultiple;	// indicates multiple diffuse are sent.

    // Material component elements:
    const SoGLLazyElement		*lazyElt;

    // Really sends stuff to GL
    void		reallySend(int index, SbBool isBetweenBeginEnd,
				   SbBool avoidFastColor);

    // Accesses and stores pointers to necessary elements
    void		accessElements(SbBool isBetweenBeginEnd,
				       SbBool avoidFastColor);
};

#endif /* _SO_MATERIAL_BUNDLE */
