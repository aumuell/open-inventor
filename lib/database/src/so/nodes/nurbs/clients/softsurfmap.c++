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
 * surfacemap.c++ - surface evaluator
 *
 * $Revision: 1.1.1.1 $
 */

#include <stdio.h>
#include <inttypes.h>   /* defines long and brethren */
#include "myassert.h"
#include "mystdio.h"
#include "mymath.h"
#include "types.h"
#include "softsurfmap.h"

_SoNurbsSurfaceMap::_SoNurbsSurfaceMap( long _type, int _rational, int _ncoords )
{ 
    type = _type; 
    rational = _rational;
    ncoords = _ncoords;
    pts = 0; 
    next = 0; 
    cacheline = 0; 
    isdefined = 0; 
    isenabled = 0;
}

_SoNurbsSurfaceMap::~_SoNurbsSurfaceMap()
{
    if( pts ) delete[] pts;
    if( cacheline ) delete[] cacheline;
}

void	_SoNurbsSurfaceMap::domain( float * ){}
void	_SoNurbsSurfaceMap::point( float * ){}
void	_SoNurbsSurfaceMap::uderiv( float * ){}
void	_SoNurbsSurfaceMap::vderiv( float * ){}
void	_SoNurbsSurfaceMap::uuderiv( float * ){}
void	_SoNurbsSurfaceMap::uvderiv( float * ){}
void	_SoNurbsSurfaceMap::vvderiv( float * ){}
void    _SoNurbsSurfaceMap::startNewTriStrip(){}
void    _SoNurbsSurfaceMap::swapTriStripVertices(){}

int
_SoNurbsSurfaceMap::isType( long t )
{
    return (type == t) ? 1 : 0;
}

void
_SoNurbsSurfaceMap::enable( void )
{
    isenabled = 1;
}

void
_SoNurbsSurfaceMap::disable( void )
{
    isenabled = 0;
}

#define SHALLOWCOPY

/*-------------------------------------------------------------------------
 * map2f - pass a desription of a surface map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceMap::map2f(
    REAL _ulower,	/* u lower domain coord		*/
    REAL _uupper,	/* u upper domain coord 	*/
    long _ustride,	/* interpoint distance		*/
    long _uorder,	/* parametric order		*/
    REAL _vlower,	/* v lower domain coord		*/
    REAL _vupper, 	/* v upper domain coord		*/
    long _vstride,	/* interpoint distance		*/
    long _vorder,	/* parametric order		*/
    REAL *_ctlpts) 	/* control points		*/
{
    assert( _ctlpts );
    assert( _ustride >= 0 );
    assert( _vstride >= 0 );
    assert( _uorder >= 0 );
    assert( _uorder <= MAXORDER );
    assert( _vorder >= 0 );
    assert( _vorder <= MAXORDER );
    assert( _ulower <= _uupper );
    assert( _vlower <= _vupper );

    isdefined = 1;
    
    if( pts == NULL ) {
	pts = new REAL[MAXCOORDS * MAXORDER * MAXORDER];
	cacheline = new _SoNurbsCacheLine[MAXCACHECURVES];
    }

    ulo = _ulower;
    uhi = _uupper;
    ustride = (int) _ustride;
    uorder = (int) _uorder;

    vlo = _vlower;
    vhi = _vupper;
    vstride = (int) _vstride;
    vorder = (int) _vorder;


#ifdef SHALLOWCOPY
    ctlpts = _ctlpts;		/* shallow copy control points */
#endif

#ifdef DEEPCOPY
    deepCopy( _ctlpts );
#endif

#ifdef HORNER
    deepCopyAndScale( _ctlpts );
#endif
}

/*---------------------------------------------------------------------------
 * setv - set v coordinate for current isoparametric line
 *---------------------------------------------------------------------------
 */

void
_SoNurbsSurfaceMap::setv( REAL _v, int index )
{
    if( !isdefined || !isenabled ) return;

    _SoNurbsCacheLine& cl = cacheline[index];
    init( vorder, vstride, _v, vlo, vhi );
    for( int i = uorder-1; i >= 0 ; i-- ) {            
	for( int j = ncoords-1; j >= 0 ; j-- ) {
	    evaluate( &ctlpts[i*ustride + j], 
		&cl.p[i][j], &cl.pv[i][j], &cl.pvv[i][j] );
	}
    }
}

/*---------------------------------------------------------------------------
 * mapeval - evaluate all maps at given point of given isoline
 *---------------------------------------------------------------------------
 */

void
_SoNurbsSurfaceMap::mapeval( REAL *domain, int index, int place )
{
    if( !isdefined || !isenabled ) return;

    _SoNurbsCacheLine& cl = cacheline[index];
    _SoNurbsCacheVal&  cv = cl.cacheval[place];
    init( uorder, MAXCOORDS, domain[0], ulo, uhi );
    for( int j = ncoords-1; j >= 0; j-- ) {
	evaluate( &cl.p[0][j], &cv.c[j], &cv.cu[j], &cv.cuu[j] );
	evaluate( &cl.pv[0][j], &cv.cv[j], &cv.cuv[j], 0 );
	evaluate( &cl.pvv[0][j], &cv.cvv[j], 0, 0 );
    }
    output( domain, index, place );
}

/*-------------------------------------------------------------------------
 * output - output a point on a map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceMap::output( REAL *_domain, int _index, int _place )
{
    if( !isdefined || !isenabled ) return;

    _SoNurbsCacheVal& val = cacheline[_index].cacheval[_place];
    domain( _domain );
    uuderiv( val.cuu );
    uvderiv( val.cuv );
    vvderiv( val.cvv );
    uderiv( val.cu );
    vderiv( val.cv );
    point( val.c );
}

void
_SoNurbsSurfaceMap::deepCopy( REAL *_pts )
{
    int i,j,k;
    REAL *p, *q, *r;
    REAL *mp = pts;

    for( p=_pts, i=0; i<uorder; i++, p += ustride ) 
	for( q=p, j=0; j<vorder; j++, q += vstride ) 
	    for( r=q, k=0; k<ncoords; k++ ) 
	        *(mp++) = *(r++);

    ctlpts = pts;
    ustride = ncoords * vorder;
    vstride = ncoords;
}

void
_SoNurbsSurfaceMap::deepCopyAndScale( REAL *_ctlpts )
{
    int nc = ncoords;
    int ou = uorder;
    int ov = vorder;
    int su = ustride;
    int sv = vstride;

    int i,j,k;
    for( j = 0; j != ov; j++ ) {
	REAL comb = Ch(ov-1, j);
	for( i = 0; i != ou; i++ ) {
	    for( k = 0; k < nc; k++ ) {
		pts[i*nc*ov+j*nc+k] = comb * _ctlpts[i*su+j*sv+k];
	    }
	}
    }

    for( i = 0;i != ou; i++ ) {
	REAL comb = Ch(ou-1, i);
	for( j = 0; j != ov; j++ ) {
	    for( k = 0; k != nc; k++ ) {
		pts[i*nc*ov+j*nc+k] *= comb;
	    }
	}
    }
    ctlpts = pts;
    ustride = ncoords * vorder;
    vstride = ncoords;
}
