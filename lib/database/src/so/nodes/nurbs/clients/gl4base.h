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
 * gl4base.h - $Revision: 1.1.1.1 $
 */

#ifndef SO_NURBS_GL4_BASE
#define SO_NURBS_GL4_BASE

#include <inttypes.h>   /* defines long and brethren */

#include "math.h"
#include "stdlib.h"
#include "nurbstess.h"
#include "gl4types.h"
#include "gl4surfeval.h"
#include "gl4curveval.h"


class _SoNurbsGL4Base : public _SoNurbsNurbsTessellator {
public:
			_SoNurbsGL4Base( _SoNurbsBasicCurveEvaluator &,
                                         _SoNurbsBasicSurfaceEvaluator & );
    virtual void	errorHandler( int );
    virtual void	bgnrender( void );
    virtual void	endrender( void );
    void		makeGLobj( int n );
    void		gl4samplingmode( void );

protected:
    int			makingGLobj;
    int			needMatrices;
    int			objnum;

private:
    void 		loadGLMatrices( void );
    void		loadSamplingMatrix( float vmat[4][4] );
    void		loadCullingMatrix( float vmat[4][4] );
    static void		grabGLMatrix( float vmat[4][4] );
    static void		transform4d( float A[4], float B[4], float mat[4][4] );
    static void		multmatrix4d( float n[4][4], float left[4][4], float right[4][4] );
};

#endif /* SO_NURBS_GL4_BASE */


