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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the _SoNurbsPrimRender class.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_PRIM_RENDER_
#define  _SO_NURBS_PRIM_RENDER_

//#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include <Inventor/SbLinear.h>
#include "head/nurbstess.h"
#include "SoPrimEvals.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: _SoNurbsPrimRender
//
//  This class is the main class for tessellating a NURBS surface for
//  primitive generation.  It contains surface and curve evaluators which
//  are used during tessellation to evaluate points on the surface.
//
//////////////////////////////////////////////////////////////////////////////

class _SoNurbsPrimRender : public _SoNurbsNurbsTessellator {

public:
    // Constructor.  Takes the action, a callback, and user data.  The action
    // is used when constructing the primitive details, and the callback
    // is called with the details.
			_SoNurbsPrimRender( SoAction *, SoNurbsPrimCB *, void * );
			~_SoNurbsPrimRender();
    void 		loadMatrices( SbMatrix &, const SbVec2s & );
    virtual void        errorHandler( int );

protected:
    _SoNurbsPrimSurfEval	surfaceEvaluator;
    _SoNurbsPrimCurvEval	curveEvaluator;

    void		loadSamplingMatrix( SbMatrix &, const SbVec2s & );
    void		loadCullingMatrix( SbMatrix & );
};

#endif /* _SO_NURBS_PRIM_RENDER_ */


