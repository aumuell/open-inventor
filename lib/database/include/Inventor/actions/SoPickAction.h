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
 |	Defines the abstract SoPickAction class
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PICK_ACTION_
#define  _SO_PICK_ACTION_

#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoSubAction.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPickAction
//
//  Base class for picking actions.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoPickAct
class SoPickAction : public SoAction {

    SO_ACTION_HEADER(SoPickAction);

  public:
    // Sets current viewport region to use for picking. Even though
    // picking may not involve a window per se, some nodes need this
    // information to determine their size and placement.
    // C-api: name=setVPReg
    void		setViewportRegion(const SbViewportRegion &newRegion);

    // Returns current viewport region
    // C-api: name=getVPReg
    const SbViewportRegion &getViewportRegion() const	{ return vpRegion; }

  SoEXTENDER public:
    // Setting this flag to FALSE disables any pick culling that might
    // take place (as in SoSeparators). This can be used for nodes
    // (such as SoArray and SoMultipleCopy) that traverse their
    // children multiple times in different locations, thereby
    // avoiding computing bounding boxes each time. (This problem is
    // very severe when each bounding box traversal also traverses the
    // children N times.) The default setting is TRUE.
    void		enableCulling(SbBool flag)	{ culling = flag; }
    SbBool		isCullingEnabled() const	{ return culling; }

  SoINTERNAL public:
    static void		initClass();

  protected:
    SbViewportRegion	vpRegion;	// Current viewport region

    // Constructor takes viewport region. See comments for the
    // setViewportRegion() method.
    SoPickAction(const SbViewportRegion &viewportRegion);

    // Destructor
    virtual ~SoPickAction();

    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

  private:
    SbBool		culling;	// Pick culling enabled?
};

#endif /* _SO_PICK_ACTION_ */
