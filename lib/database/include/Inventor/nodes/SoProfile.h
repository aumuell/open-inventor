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
 |   $Revision $
 |
 |   Description:
 |	This file defines the SoProfile node class.
 |
 |   Author(s)		: Thad Beier, Dave Immel, Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PROFILE_
#define  _SO_PROFILE_

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoProfile
//
//  Abstract base class of 2D profiles.  These profiles are used as trim
//  curves for nurbs and bevels for 3D text.  The two (so far) subclasses
//  are SoLinearProfile and SoNurbsProfile
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoProf
// C-api: public=index, linkage
class SoProfile : public SoNode{

    SO_NODE_ABSTRACT_HEADER(SoProfile);

  public:
    enum Profile {
	START_FIRST    = SoProfileElement::START_FIRST,
	START_NEW      = SoProfileElement::START_NEW,
	ADD_TO_CURRENT = SoProfileElement::ADD_TO_CURRENT
    };

    // Fields
    SoMFInt32		index;	      // Number of vertices per profile
    SoSFEnum		linkage;      // connectivity of this curve and others

  SoEXTENDER public:
    // These action methods are implemented for profiles. Each of them
    // just adds the profile to the current state.
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	pick(SoPickAction *action);

    // Returns a profile as a trim curve.  numKnots is zero if the
    // curve is linear. floatsPerVec will be either 2 or 3 depending
    // on whether the curve is non-rational or rational.
    virtual void	getTrimCurve(SoState *state, int32_t &numPoints,
				     float *&points, int &floatsPerVec,
				     int32_t &numKnots, float *&knotVector) = 0;

    // Returns vertices approximating the profile
    virtual void	getVertices(SoState *state, int32_t &nVertices,
				    SbVec2f *&vertices) = 0;

  SoINTERNAL public:
    static void		initClass();

  protected:
    SoProfile();

    virtual ~SoProfile();
};

#endif /* _SO_PROFILE_ */
