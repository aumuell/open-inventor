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
 * softsurfacemap.h - surface evaluator
 *
 * $Revision: 1.1.1.1 $
 */

#ifndef SO_NURBS_SOFTSURFMAP
#define SO_NURBS_SOFTSURFMAP

#include <inttypes.h>   /* defines long and brethren */

#include "mystdio.h"
#include "mymath.h"
#include "defines.h"
#include "isocurveeval.h"
#include "types.h"

#define MAXCACHED	100
#define MAXCACHECURVES	3

struct _SoNurbsCacheVal {
    REAL		c[MAXCOORDS];
    REAL		cu[MAXCOORDS];
    REAL		cv[MAXCOORDS];
    REAL		cuu[MAXCOORDS];
    REAL		cuv[MAXCOORDS];
    REAL		cvv[MAXCOORDS];
};

struct _SoNurbsCacheLine {
    REAL		p[MAXORDER][MAXCOORDS];
    REAL		pv[MAXORDER][MAXCOORDS];
    REAL		pvv[MAXORDER][MAXCOORDS];
    _SoNurbsCacheVal		cacheval[MAXCACHED];
};

class _SoNurbsSurfaceMap : public _SoNurbsIsocurveEvaluator {
public:
			_SoNurbsSurfaceMap( long, int, int );
    virtual		~_SoNurbsSurfaceMap();
    int			isType( long );

    void		disable( void );
    void		enable( void );
    void		map2f( REAL, REAL, long, long, 
			       REAL, REAL, long, long, REAL * );
    void		setv( REAL, int );
    void		mapeval( REAL *, int, int );
    void		output(  REAL *, int, int );

    _SoNurbsSurfaceMap *	next;

    // Methods used to handle triangle strips
    virtual void        startNewTriStrip();
    virtual void	swapTriStripVertices();
    
protected:
    virtual void	domain( float * );
    virtual void	point( float * );
    virtual void	uderiv( float * );
    virtual void	vderiv( float * );
    virtual void	uuderiv( float * );
    virtual void	uvderiv( float * );
    virtual void	vvderiv( float * );

private:
    void		deepCopy( REAL *_pts );
    void		deepCopyAndScale( REAL *_ctlpts );

    long		type;
    int			rational;
    int			ncoords;
    REAL *		ctlpts;
    REAL *		pts;
    int			ustride, vstride;
    int			uorder, vorder;
    REAL		ulo, vlo;
    REAL		uhi, vhi;
    _SoNurbsCacheLine *	cacheline;
    int			isenabled;
    int			isdefined;
};

#endif /* SO_NURBS_SOFTSURFMAP */
