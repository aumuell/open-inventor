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
 |	This file defines the SoSwitch node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SWITCH_
#define  _SO_SWITCH_

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoGroup.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSwitch
//
//  Switch group node: traverses only the child indicated by integer
//  "whichChild" field. If this field is SO_SWITCH_NONE, no children
//  are traversed, while SO_SWITCH_INHERIT means inherit the index
//  from the current switch state, doing a modulo operation to make
//  sure the child index is within the correct bounds of the switch
//  node.
//
//////////////////////////////////////////////////////////////////////////////

#define SO_SWITCH_NONE		(-1)	/* Don't traverse any children	*/
#define SO_SWITCH_INHERIT	(-2)	/* Inherit value from state	*/
#define SO_SWITCH_ALL		(-3)	/* Traverse all children	*/

// C-api: public=whichChild
class SoSwitch : public SoGroup {

    SO_NODE_HEADER(SoSwitch);

  public:
    // Fields
    SoSFInt32		whichChild;	// Child to traverse

    // Default constructor
    SoSwitch();

    // Constructor that takes approximate number of children
    // C-api: name=CreateN
    SoSwitch(int nChildren);

    // Overrides method in SoNode to return FALSE if there is no
    // selected child or the selected child does not affect the state.
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
    virtual void	write(SoWriteAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Generic traversal of children
    void		traverseChildren(SoAction *action);

    virtual ~SoSwitch();

  private:
    // Traverses correct child. If matchIndex >=0, child index must match
    void		doChild(SoAction *action, int matchIndex = -1);
};

#endif /* _SO_SWITCH_ */
