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
 |	This file defines the SoNurbsSurface node class.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_SURFACE_
#define  _SO_NURBS_SURFACE_

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNurbsSurface
//
//  NURBS surface. The surface consists of two knot vectors and orders for
//  the U and V parametric directions.  The control points for the surface
//  are obtained from the current coordinates.  There are also orders and
//  knot vectors for the S and T (texture) coordinates
//
//////////////////////////////////////////////////////////////////////////////

class SoState;
class SoPrimitiveVertex;
class _SoNurbsNurbsTessellator;

// C-api: prefix=SoNurbs
// C-api: public=numUControlPoints, numVControlPoints
// C-api: public=numSControlPoints, numTControlPoints
// C-api: public=uKnotVector, vKnotVector
// C-api: public=sKnotVector, tKnotVector
class SoNurbsSurface : public SoShape {

    SO_NODE_HEADER(SoNurbsSurface);

  public:
    // Fields
    SoSFInt32		numUControlPoints; // Number of control points in
    SoSFInt32		numVControlPoints; // the U and V directions.
    SoSFInt32		numSControlPoints; // Number of control points in
    SoSFInt32		numTControlPoints; // the S and T directions
    SoMFFloat		uKnotVector;       // The knot vectors in the U and
    SoMFFloat		vKnotVector;       // V directions.
    SoMFFloat		sKnotVector;       // The knot vectors in the S and
    SoMFFloat		tKnotVector;       // T directions.

    // Constructor
    SoNurbsSurface();

  SoEXTENDER public:
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

    void                sendPrimitive(SoAction *, SoPrimitiveVertex *);

  protected:
    // Generates triangles representing a surface
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of surface
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    ~SoNurbsSurface();

  private:
    // Renders the surface
    void                drawNURBS(_SoNurbsNurbsTessellator *render, SoState *state,
				  SbBool doTextures);

    // Matrix operations for level of detail and culling
    void                calcTotalMatrix( SoState *state, SbMatrix &totalMat );
    void                multMatrix4d( SbMatrix &n, SbMatrix left, 
				      SbMatrix right );

    // This callback gets details from the NURBS library
    static void         primCB(SoAction *, SoPrimitiveVertex *, void *);
};

#endif /* _SO_NURBS_SURFACE_ */
