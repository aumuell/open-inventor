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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      _SoNurbsPrimCurvEval
 |      _SoNurbsPrimSurfEval
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


/* Polynomial Evaluator Interface */

#include <stdio.h>
#include "SoAddPrefix.h"
#include <Inventor/actions/SoAction.h>
#include <Inventor/nodes/SoShape.h>
#include "SoPrimEvals.h"
#include "SoPrimMaps.h"


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: internal

_SoNurbsPrimCurvEval::_SoNurbsPrimCurvEval( void ) 

//
////////////////////////////////////////////////////////////////////////
{ 
    v3CurveMap        = new _SoNurbsPrimV3CurveMap;
    v4CurveMap        = new _SoNurbsPrimV4CurveMap;
    t2CurveMap        = new _SoNurbsPrimTex2CurveMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: internal

_SoNurbsPrimCurvEval::~_SoNurbsPrimCurvEval( void ) 

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
_SoNurbsPrimCurvEval::initialize(SoAction *act, SoNurbsPrimCB *cb, void *data )

//
////////////////////////////////////////////////////////////////////////
{
    v3CurveMap->initialize( act, cb, data );
    v4CurveMap->initialize( act, cb, data );
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
_SoNurbsPrimCurvEval::bgnline( void )

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
_SoNurbsPrimCurvEval::endline( void )

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

_SoNurbsPrimSurfEval::_SoNurbsPrimSurfEval()

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap        = new _SoNurbsPrimV3SurfaceMap;
    v4SurfaceMap        = new _SoNurbsPrimV4SurfaceMap;
    t2SurfaceMap        = new _SoNurbsPrimTex2SurfaceMap;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: internal

_SoNurbsPrimSurfEval::~_SoNurbsPrimSurfEval()

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
_SoNurbsPrimSurfEval::initialize(SoAction *act, SoNurbsPrimCB *cb, void *data )

//
////////////////////////////////////////////////////////////////////////
{
    v3SurfaceMap->initialize( act, cb, data );
    v4SurfaceMap->initialize( act, cb, data );
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
_SoNurbsPrimSurfEval::bgnline( void )

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
_SoNurbsPrimSurfEval::endline( void )

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
_SoNurbsPrimSurfEval::bgnclosedline( void )

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
_SoNurbsPrimSurfEval::endclosedline( void )

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
_SoNurbsPrimSurfEval::bgntmesh()

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
_SoNurbsPrimSurfEval::swaptmesh( void )

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
_SoNurbsPrimSurfEval::endtmesh( void )

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
_SoNurbsPrimSurfEval::bgnqstrip( void )

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
_SoNurbsPrimSurfEval::endqstrip( void )

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when an grid of quadstrips is
//    generated.
//
// Use: protected

void
_SoNurbsPrimSurfEval::mapmesh2f( long, long umin, long umax,
        long vmin, long vmax )

//
////////////////////////////////////////////////////////////////////////
{
    makeMesh(umin, umax, vmin, vmax);
}

