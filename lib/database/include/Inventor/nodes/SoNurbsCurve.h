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
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoNurbsCurve node class.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_CURVE_
#define  _SO_NURBS_CURVE_

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNurbsCurve
//
//  NURBS curve. The curve consists of a knot vector and order for
//  the U parametric directions.  The control points for the curve
//  are obtained from the current coordinates.
//
//////////////////////////////////////////////////////////////////////////////

class SoState;
class SoPrimitiveVertex;
class _SoNurbsNurbsTessellator;

// C-api: prefix=SoCurve
// C-api: public=numControlPoints, knotVector
class SoNurbsCurve : public SoShape {

    SO_NODE_HEADER(SoNurbsCurve);

  public:
    // Fields
    SoSFInt32		numControlPoints; // Number of control points
    SoMFFloat		knotVector;       // The knot vector

    // Constructor
    SoNurbsCurve();

  SoEXTENDER public:
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

    // Redefine this to tell open caches that they contain lines
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);

  SoINTERNAL public:
    static void		initClass();

    void                sendPrimitive(SoAction *, SoPrimitiveVertex *);

  protected:
    // Generates triangles representing a sphere
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of curve
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    ~SoNurbsCurve();

  private:

    void                drawNURBS(_SoNurbsNurbsTessellator *render, SoState *state);

    void                calcTotalMatrix( SoState *state, SbMatrix &totalMat );

    void                multMatrix4d ( SbMatrix &n, SbMatrix left,
                              SbMatrix right );

    // This callback gets details from the NURBS library
    static void         primCB(SoAction *, SoPrimitiveVertex *, void *);
};

#endif /* _SO_NURBS_CURVE_ */
