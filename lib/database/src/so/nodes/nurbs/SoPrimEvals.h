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
 |	This file defines following classes:
 |          _SoNurbsPrimCurvEval
 |          _SoNurbsPrimSurfEval
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_PRIM_EVALS_
#define  _SO_NURBS_PRIM_EVALS_

#include "clients/softcurveval.h"
#include "clients/softsurfeval.h"
#include "SoPrimMaps.h"

class _SoNurbsPrimCurvEval : public _SoNurbsCurveEvaluator {
public:
			_SoNurbsPrimCurvEval( void );
			~_SoNurbsPrimCurvEval( void );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );

protected:
    _SoNurbsPrimV3CurveMap    *v3CurveMap;
    _SoNurbsPrimV4CurveMap    *v4CurveMap;
    _SoNurbsPrimTex2CurveMap  *t2CurveMap;

    //
    // These are the callback routines for getting lines back.
    //
    void		bgnline( void );
    void		endline( void );
};


class _SoNurbsPrimSurfEval : public _SoNurbsSurfaceEvaluator {
public:
			_SoNurbsPrimSurfEval();
                        ~_SoNurbsPrimSurfEval( void );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );

protected:
    _SoNurbsPrimV3SurfaceMap  *v3SurfaceMap;
    _SoNurbsPrimV4SurfaceMap  *v4SurfaceMap;
    _SoNurbsPrimTex2SurfaceMap *t2SurfaceMap;

    //
    // These are the callback routines for getting triangles back.
    //
    void	 	bgnline( void );
    void	 	endline( void );
    void	 	bgnclosedline( void );
    void	 	endclosedline( void );
    void	 	bgntmesh( void );
    void	 	swaptmesh( void );
    void	 	endtmesh( void );
    void	 	bgnqstrip( void );
    void	 	endqstrip( void );
    void                mapmesh2f( long, long, long, long, long );};


#endif /* _SO_NURBS_PRIM_EVALS_ */
