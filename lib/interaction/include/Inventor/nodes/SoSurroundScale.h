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
 |	This file defines the SoSurroundScale node class.
 |
 |   Author(s)		: Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SURROUND_SCALE_
#define  _SO_SURROUND_SCALE_

#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoTransformation.h>
#include <Inventor/fields/SoSFInt32.h>

class SoFullPath;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSurroundScale
//
//  Appends a transform to the localMatrix so that a default size  cube will 
//  surround the objects specified by its fields.
//
//  To determine what to surround, the node looks at the current path in
//  the action.  An SoGetBoundingBoxAction is applied to the node that is
//  'numNodesUpToContainer' nodes above this node in the path.
//  The action will be told to reset the bounding box upon traversal of the
//  node that is 'numNodesUpToReset' nodes above this node in the path.
//
//  For example, when a trackballManip wants to surround the objects it
//  is going to move, the scene graph will look something like this:
//
//               Separator
//                |
//     -------------------------
//     |                       |
//   trackballManip       subGraphOfObjectsThatWilMove
//     |
//    trackballDragger
//     |
//    topSeparator(top part within of the dragger)
//     |
//    -----------------------------------
//    |            |                    |
//  motionMatrix  surroundScale     parts of the dragger.
//
//   The manip will set the fields on the surround scale node to be:
//   numNodesUpToContainer = 4;
//   numNodesUpToReset = 3;
//
//  The action will therefore be applied to the Separator, and will be
//  reset after traversing the trackballManip.
//  So the surroundScale will surround the objects below 'separator' and
//  not including 'trackballManip,' producing the desired effect.
//
//  Note that, therefore, designers of draggers and manipulators which use
//  this node should base their models on default size cubes (2 by 2 by 2).
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=numNodesUpToContainer,numNodesUpToReset
class SoSurroundScale : public SoTransformation {

    SO_NODE_HEADER(SoSurroundScale);

  public:

    // Constructor
    SoSurroundScale();

    // Fields
    SoSFInt32		numNodesUpToContainer;
    SoSFInt32		numNodesUpToReset;

    // If you call this, then next time through the node will re-calculate
    // it's cached matrix, translation and scale values.
    void invalidate();

  SoEXTENDER public:
    // Called by actions that need to change the state with a new matrix.
    void		doAction(SoAction *action);

    void setDoingTranslations( SbBool doEm ) { doTranslations = doEm; }
    SbBool isDoingTranslations() { return doTranslations; }

  SoINTERNAL public:
    static void		initClass();

  protected:

    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	pick(SoPickAction *action);

    void updateMySurroundParams( SoAction *action, const SbMatrix &inv);

    void setIgnoreInBbox( SbBool newVal ) { ignoreInBbox = newVal; }
    SbBool isIgnoreInBbox() { return ignoreInBbox; }

    SbVec3f    cachedScale;
    SbVec3f    cachedInvScale;
    SbVec3f    cachedTranslation;
    SbBool     cacheOK;

    SbBool doTranslations;

    virtual ~SoSurroundScale();

  private:
    SbBool ignoreInBbox;
};

#endif /* _SO_SURROUND_SCALE_ */
