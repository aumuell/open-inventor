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
 |   $Revision $
 |
 |   Classes:
 |      _SoNurbsCurveRenderEval
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


/* Polynomial Evaluator Interface */
#include "SoAddPrefix.h"
#include "SoCurveRenderEval.h"
#include "SoCurveMaps.h"

_SoNurbsCurveRenderEval::_SoNurbsCurveRenderEval( void ) 
{ 
    v3Map        = new _SoNurbsCurveV3Map;
    v4Map        = new _SoNurbsCurveV4Map;

    numVertices       = 0;
    numAllocVertices  = 0;
    vertices          = NULL;
}

_SoNurbsCurveRenderEval::~_SoNurbsCurveRenderEval( void ) 
{ 
    delete[] vertices;
}

void
_SoNurbsCurveRenderEval::initialize()
{
    v3Map->initialize( &numVertices, &numAllocVertices, &vertices );
    v4Map->initialize( &numVertices, &numAllocVertices, &vertices );
    addMap( v3Map );
    addMap( v4Map );
}

void
_SoNurbsCurveRenderEval::getVertices( int32_t &nVerts, SbVec2f *&verts )
{
    nVerts = numVertices;
    verts  = vertices;
}


void _SoNurbsCurveRenderEval::bgnline( void ){}
void _SoNurbsCurveRenderEval::endline( void ){}
