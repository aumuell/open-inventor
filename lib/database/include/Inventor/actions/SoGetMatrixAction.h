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
 |	Defines the SoGetMatrixAction class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GET_MATRIX_ACTION_
#define  _SO_GET_MATRIX_ACTION_

#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/SbViewportRegion.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGetMatrixAction
//
//  For computing a cumulative transformation matrix and its inverse,
//  along with a cumulative texture transformation matrix and its inverse.
//
//  This action is unlike most others in that it does not traverse
//  downwards. When called on a node, it computes the matrix for just
//  that node. (This makes sense for transformation nodes, but not for
//  others, really.) When called on a path, it gathers the
//  transformation info for all nodes in the path and those that
//  affect nodes in the path, but it stops when it hits the last node
//  in the path; it does not traverse downwards from it as other
//  actions (such as rendering) do. This behavior makes the most sense
//  for this action.
//
//  EXTENDERS:  The model and texture matrix elements are not enabled
//  for this action.  Your transformation nodes must use the
//  getMatrix/getInverse calls and directly modify the matrices stored
//  in the action.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMxAct
class SoGetMatrixAction : public SoAction {

    SO_ACTION_HEADER(SoGetMatrixAction);

  public:
    // Constructor
    SoGetMatrixAction(const SbViewportRegion &newRegion);

    // Destructor
    virtual ~SoGetMatrixAction();

    // Sets current viewport region to use for action
    // C-api: name=setVPReg
    void		setViewportRegion(const SbViewportRegion &newRegion);

    // Returns current viewport region
    // C-api: name=getVPReg
    const SbViewportRegion &getViewportRegion() const	{ return vpRegion; }

    // Returns cumulative transformation matrix and inverse matrix
    // Extenders:  your transformation nodes should get these and
    // directly modify them.  Users:  you should treat these as
    // constants.
    // C-api: name=getMx
    SbMatrix &		getMatrix() 		{ return ctm; }
    // C-api: name=getInv
    SbMatrix &		getInverse()		{ return inv; }

    // Returns cumulative texture transformation matrix and inverse matrix
    // Extenders:  your textureTransformation nodes should get these
    // and directly modify them.  Users:  you should treat these as
    // constants.
    // C-api: name=getTexMx
    SbMatrix &		getTextureMatrix() 	{ return texCtm; }
    // C-api: name=getTexInv
    SbMatrix &		getTextureInverse()	{ return texInv; }

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

  private:
    SbMatrix		ctm;		// Cumulative transformation matrix
    SbMatrix		inv;		// Inverse transformation matrix
    SbMatrix		texCtm;		// Cumulative texture transf matrix
    SbMatrix		texInv;		// Inverse texture transf matrix
    SbViewportRegion	vpRegion;	// Current viewport region
};

#endif /* _SO_GET_MATRIX_ACTION_ */
