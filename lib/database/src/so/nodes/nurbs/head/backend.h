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

#ifndef __glubackend_h_
#define __glubackend_h_

#include "../SoAddPrefix.h"

/*
 * backend.h - $Revision: 1.1.1.1 $
 */

#include "trimvertex.h"
#include "gridvertex.h"
#include "gridtrimvertex.h"

class BasicCurveEvaluator;
class BasicSurfaceEvaluator;

class Backend {
private:
    BasicCurveEvaluator&	curveEvaluator;
    BasicSurfaceEvaluator&	surfaceEvaluator;
public:
  			Backend( BasicCurveEvaluator &c, BasicSurfaceEvaluator& e )
			: curveEvaluator(c), surfaceEvaluator(e) {}

    /* surface backend routines */
    void		bgnsurf( int, int, long  );
    void		patch( REAL, REAL, REAL, REAL );
    void		surfpts( long, REAL *, long, long, int, int,
          			 REAL, REAL, REAL, REAL );
    void		surfbbox( long, REAL *, REAL * );
    void		surfgrid( REAL, REAL, long, REAL, REAL, long ); 
    void		surfmesh( long, long, long, long ); 
    void		bgntmesh( char * );
    void		endtmesh( void );
    void		swaptmesh( void );
    void		tmeshvert( GridTrimVertex * );
    void		tmeshvert( TrimVertex * );
    void		tmeshvert( GridVertex * );
    void		linevert( TrimVertex * );
    void		linevert( GridVertex * );
    void		bgnoutline( void );
    void		endoutline( void );
    void		endsurf( void );
    void		triangle( TrimVertex*, TrimVertex*, TrimVertex* );

    /* curve backend routines */
    void		bgncurv( void );
    void		segment( REAL, REAL );
    void		curvpts( long, REAL *, long, int, REAL, REAL );
    void		curvgrid( REAL, REAL, long );
    void		curvmesh( long, long );
    void		curvpt( REAL  );  
    void		bgnline( void );
    void		endline( void );
    void		endcurv( void );
private:
#ifndef NOWIREFRAME
    int			wireframetris;
    int			wireframequads;
    int			npts;
    REAL		mesh[3][4];
    int			meshindex;
#endif
};

#endif /* __glubackend_h_ */
