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
 |	This file defines the SoMultipleCopy node class.
 |
 |   Author(s)		: Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MULTIPLE_COPY_
#define  _SO_MULTIPLE_COPY_

#include <Inventor/fields/SoMFMatrix.h>
#include <Inventor/nodes/SoGroup.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMultipleCopy
//
//  MultipleCopy group node: children are traversed for each matrix in
//  the multiple-value "matrix" field, after transforming the current
//  model matrix by that matrix. State is saved before traversing all
//  children each time and restored afterwards.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMultCopy
// C-api: public=matrix
class SoMultipleCopy : public SoGroup {

    SO_NODE_HEADER(SoMultipleCopy);

  public:
    // Fields
    SoMFMatrix		matrix;	// Set of matrices to apply to children

    SoMultipleCopy();

    // Overrides default method on SoNode to return FALSE since these
    // nodes are effectively separators
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
    virtual ~SoMultipleCopy();
};

#endif /* _SO_MULTIPLE_COPY_ */
