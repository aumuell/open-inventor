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
 |	This file defines the SoArray node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ARRAY_
#define  _SO_ARRAY_

#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoGroup.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoArray
//
//  Array group node: creates an IxJxK array. Each element in the
//  array is drawn by traversing all of the children of this node,
//  saving and restoring state before and after each element. I, J,
//  and K are given by the numElements1, numElements2, and
//  numElements3 fields. The 3D vector separating the centers of
//  adjacent elements in each of the three directions is given by
//  separation1, separation2, and separation3. The origin field
//  indicates whether the array is positioned with the current
//  object-space origin at the center of the first element, the last
//  element, or at the center of all elements.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=numElements1, numElements2, numElements3
// C-api: public=separation1, separation2, separation3, origin
class SoArray : public SoGroup {

    SO_NODE_HEADER(SoArray);

  public:

    // Fields
    SoSFShort		numElements1;	// Number of elements in 1st direction
    SoSFShort		numElements2;	// Number of elements in 2nd direction
    SoSFShort		numElements3;	// Number of elements in 3rd direction

    SoSFVec3f		separation1;	// Separation vector in 1st direction
    SoSFVec3f		separation2;	// Separation vector in 2nd direction
    SoSFVec3f		separation3;	// Separation vector in 3rd direction

    enum Origin {		// Array origin:
	FIRST,				// Origin at first element
	CENTER,				// Origin at center of elements
	LAST				// Origin at last element
    };

    SoSFEnum		origin;		// Base point

    // Constructor and destructor
    SoArray();

    // Overrides default method on SoNode to return FALSE since arrays
    // are effectively separators
    virtual SbBool	affectsState() const;

  SoEXTENDER public:
    // Implement actions
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	pick(SoPickAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	handleEvent(SoHandleEventAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	search(SoSearchAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoArray();
};

#endif /* _SO_ARRAY_ */
