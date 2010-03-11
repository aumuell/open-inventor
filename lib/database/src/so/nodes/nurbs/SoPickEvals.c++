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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      _SoNurbsPickCurvEval
 |      _SoNurbsPickSurfEval
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


/* Polynomial Evaluator Interface */

#include <stdio.h>
#include "SoAddPrefix.h"
#include <Inventor/actions/SoRayPickAction.h>
#include "SoPickEvals.h"
#include "SoPickMaps.h"


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: internal

_SoNurbsPickCurvEval::_SoNurbsPickCurvEval( void ) 

//
////////////////////////////////////////////////////////////////////////
{ 
    v3CurveMap        = new _SoNurbsPickV3CurveMap;
    v4CurveMap        = new _SoNurbsPickV4CurveMap;
    t2CurveMap        = new _SoNurbsPickTex2CurveMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: internal

_SoNurbsPickCurvEval::~_SoNurbsPickCurvEval( void ) 

//
////////////////////////////////////////////////////////////////////////
{ 
//    delete t2CurveMap;
//    delete v4CurveMap;
//    delete v3CurveMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method initializes the callback routines for getting 
//    vertices and lines back from the NURBS library.
//
// Use: internal

void
_SoNurbsPickCurvEval::initialize(SoRayPickAction *pa)

//
////////////////////////////////////////////////////////////////////////
{
    v3CurveMap->initialize( pa );
    v4CurveMap->initialize( pa );
    addMap( v3CurveMap );
    addMap( v4CurveMap );
    addMap( t2CurveMap );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a bgnline call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickCurvEval::bgnline( void )

//
////////////////////////////////////////////////////////////////////////
{
    v3CurveMap->startNewCurve();
    v4CurveMap->startNewCurve();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an endline call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickCurvEval::endline( void )

//
////////////////////////////////////////////////////////////////////////
{
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: internal

_SoNurbsPickSurfEval::_SoNurbsPickSurfEval()

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap        = new _SoNurbsPickV3SurfaceMap;
    v4SurfaceMap        = new _SoNurbsPickV4SurfaceMap;
    t2SurfaceMap        = new _SoNurbsPickTex2SurfaceMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: internal

_SoNurbsPickSurfEval::~_SoNurbsPickSurfEval()

//
////////////////////////////////////////////////////////////////////////
{
//    delete t2SurfaceMap;
//    delete v4SurfaceMap;
//    delete v3SurfaceMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method initializes the callback routines for getting 
//    vertices and faces back from the NURBS library.
//
// Use: internal

void
_SoNurbsPickSurfEval::initialize(SoRayPickAction *pa)

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap->initialize( pa );
    v4SurfaceMap->initialize( pa );
    addMap( v3SurfaceMap );
    addMap( v4SurfaceMap );
    addMap( t2SurfaceMap );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a bgnline call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::bgnline( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an endline call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::endline( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a bgnclosedline call
//    is generated by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::bgnclosedline( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an endclosedline call
//    is generated by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::endclosedline( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a bgntmesh call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::bgntmesh( void )

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap->startNewTriStrip();
    v4SurfaceMap->startNewTriStrip();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a swaptmesh call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::swaptmesh( void )

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap->swapVertices();
    v4SurfaceMap->swapVertices();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an endtmesh call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::endtmesh( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a bgnqstrip call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::bgnqstrip( void )

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap->startNewTriStrip();
    v4SurfaceMap->startNewTriStrip();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an endqstrip call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickSurfEval::endqstrip( void )

//
////////////////////////////////////////////////////////////////////////
{
}

