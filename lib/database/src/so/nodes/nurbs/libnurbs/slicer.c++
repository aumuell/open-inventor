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
 * slicer.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mystdio.h"
#include "myassert.h"
#include "bufpool.h"
#include "slicer.h"
#include "backend.h"
#include "arc.h"
#include "gridtrimvertex.h"
#include "trimvertex.h"
#include "varray.h"

Slicer::Slicer( Backend &b ) 
	: CoveAndTiler( b ), Mesher( b ), backend( b )
{
}

Slicer::~Slicer()
{
}

void
Slicer::setisolines( int x )
{
    isolines = x;
}

void
Slicer::setstriptessellation( REAL x, REAL y )
{
    assert(x > 0 && y > 0);
    du = x;
    dv = y;
    setDu( du );
}

void
Slicer::slice( Arc_ptr loop )
{
    loop->markverts();

    Arc_ptr extrema[4];
    loop->getextrema( extrema );

    unsigned int npts = loop->numpts();
    TrimRegion::init( npts, extrema[0] );

    Mesher::init( npts );

    long ulines = uarray.init( du, extrema[1], extrema[3] );

    Varray varray;
    long vlines = varray.init( dv, extrema[0], extrema[2] );
    long botv = 0;
    long topv;
    TrimRegion::init( varray.varray[botv] );
    getGridExtent( &extrema[0]->pwlArc->pts[0], &extrema[0]->pwlArc->pts[0] );

    for( long quad=0; quad<varray.numquads; quad++ ) {
	backend.surfgrid( uarray.uarray[0], 
		       uarray.uarray[ulines-1], 
	 	       ulines-1, 
		       varray.vval[quad], 
		       varray.vval[quad+1], 
		       varray.voffset[quad+1] - varray.voffset[quad] );

	for( long i=varray.voffset[quad]+1; i <= varray.voffset[quad+1]; i++ ) {
    	    topv = botv++;
    	    advance( topv - varray.voffset[quad], 
		     botv - varray.voffset[quad], 
		     varray.varray[botv] );
	    if( i == vlines )
		getPts( extrema[2] );
	    else
		getPts( backend );
	    getGridExtent();
            if( isolines ) {
	        outline();
	    } else {
		if( canTile() ) 
		    coveAndTile();
		else
		    mesh();
	    }
        }
   }
}


void
Slicer::outline( void )
{
    GridTrimVertex upper, lower;
    Hull::init( );

    backend.bgnoutline();
    while( (nextupper( &upper )) ) {
	if( upper.isGridVert() )
	    backend.linevert( upper.g );
	else
	    backend.linevert( upper.t );
    }
    backend.endoutline();

    backend.bgnoutline();
    while( (nextlower( &lower )) ) {
	if( lower.isGridVert() )
	    backend.linevert( lower.g );
	else
	    backend.linevert( lower.t );
    }
    backend.endoutline();
}


void
Slicer::outline( Arc_ptr jarc )
{
    jarc->markverts();

    if( jarc->pwlArc->npts >= 2 ) {
	backend.bgnoutline();
	for( int j = jarc->pwlArc->npts-1; j >= 0; j--  )
	    backend.linevert( &(jarc->pwlArc->pts[j]) );
	backend.endoutline();
    }
}

