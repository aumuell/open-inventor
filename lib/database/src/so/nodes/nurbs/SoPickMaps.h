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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the curve and surface map classes used for
 |      receiving tessellated primitives from the nurbs library to
 |      perform picking.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_PICK_MAPS_
#define  _SO_NURBS_PICK_MAPS_

#include <math.h>
#include <stdlib.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoPrimitiveVertex.h>
#include "clients/softcurvemaps.h"
#include "clients/softsurfacemaps.h"
#include "clients/gl4types.h"

class SoRayPickAction;
class SoShape;

class _SoNurbsPickV3CurveMap : public _SoNurbsVertex3CurveMap
{
public:
                        _SoNurbsPickV3CurveMap(){}
    virtual             ~_SoNurbsPickV3CurveMap();
    void		point( float * );
    void                initialize( SoRayPickAction *act );
    void                startNewCurve() {ptIndex = 0;}

protected:
    SoRayPickAction     *pickAction;
    SbVec3f     	CP[2];
    SbVec2f		TP[2];   // Texture Coordinates
    int                 ptIndex;
    void                intersectLine();
};

class _SoNurbsPickV4CurveMap : public _SoNurbsVertex4CurveMap
{
public:
                        _SoNurbsPickV4CurveMap(){}
    virtual             ~_SoNurbsPickV4CurveMap();
    void		point( float * );
    void                initialize( SoRayPickAction *act );
    void                startNewCurve() {ptIndex = 0;}

protected:
    SoRayPickAction     *pickAction;
    SbVec3f     	CP[2];
    SbVec2f		TP[2];   // Texture Coordinates
    int                 ptIndex;
    void                intersectLine();
};

class _SoNurbsPickTex2CurveMap : public _SoNurbsCurveMap
{
public:
                        _SoNurbsPickTex2CurveMap();
                        ~_SoNurbsPickTex2CurveMap();
    virtual void        point( float * );
    virtual void        uderiv( float * );
    virtual void        uuderiv( float * );
};

class _SoNurbsPickV3SurfaceMap : public _SoNurbsVertex3SurfaceMap
{

public:
                        _SoNurbsPickV3SurfaceMap(){}
    virtual             ~_SoNurbsPickV3SurfaceMap();
    void		point( float *p );
    void                initialize( SoRayPickAction *act );
    void                startNewTriStrip();
    void                swapVertices();

protected:
    SoRayPickAction     *pickAction;
    _SoNurbsSurfPartial	SP[3];
    SbVec2f		TP[3];   // Texture Coordinates
    SoPrimitiveVertex   primVertices[3];
    int                 cacheIndices[2], curCacheIndex, curPrimIndex;
    SbBool              cacheReady;
    
    void                intersectTriangle();
    void                copyPartials( _SoNurbsSurfPartial &from,
                                      _SoNurbsSurfPartial &to );
};

class _SoNurbsPickV4SurfaceMap : public _SoNurbsVertex4SurfaceMap
{
public:
                        _SoNurbsPickV4SurfaceMap(){}
    virtual             ~_SoNurbsPickV4SurfaceMap();
    void		point( float *p );
    void                initialize( SoRayPickAction *act );
    void                startNewTriStrip();
    void                swapVertices();

protected:
    SoRayPickAction     *pickAction;
    _SoNurbsSurfPartial	SP[3];
    SbVec2f		TP[3];   // Texture Coordinates
    int                 cacheIndices[2], curCacheIndex, curPrimIndex;
    SbBool              cacheReady;

    void                intersectTriangle();
    void                copyPartials( _SoNurbsSurfPartial &from,
                                      _SoNurbsSurfPartial &to );
};

class _SoNurbsPickTex2SurfaceMap : public _SoNurbsSurfaceMap
{
public:
                        _SoNurbsPickTex2SurfaceMap() : _SoNurbsSurfaceMap( N_T2D, 0, 2 ) {}
                        ~_SoNurbsPickTex2SurfaceMap();
    virtual void        point( float *p );
    virtual void        uderiv( float * );
    virtual void        vderiv( float * );
    virtual void        uuderiv( float * );
    virtual void        uvderiv( float * );
    virtual void        vvderiv( float * );
};

#endif  /*_SO_NURBS_PICK_MAPS_ */


