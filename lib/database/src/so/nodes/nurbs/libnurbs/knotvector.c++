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
 * knotvector.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mystdio.h"
#include "myassert.h"
#include "knotvector.h"
#include "defines.h"


void Knotvector::init( long _knotcount, long _stride, long _order, INREAL *_knotlist )
{
    knotcount = _knotcount; 
    stride = _stride; 
    order = _order; 
    knotlist = new Knot[_knotcount];
    assert( knotlist );

    for( int i = 0; i != _knotcount; i++ )
        knotlist[i] = (Knot) _knotlist[i]; 
}

Knotvector::Knotvector( void )
{
    knotlist = 0;
}

Knotvector::~Knotvector( void )
{
    if( knotlist ) delete[] knotlist;
}

int Knotvector::validate( void )
{
   /* kindex is used as an array index so subtract one first, 
     * this propagates throughout the code so study carefully */
    long	kindex = knotcount-1;

    if( order < 1 || order > MAXORDER ) {
	// spline order un-supported
	return( 1 );
    }

    if( knotcount < (2 * order) ) {
	// too few knots
	return( 2 );
    }

    if( identical( knotlist[kindex-(order-1)], knotlist[order-1]) ) {
	// valid knot range is empty 
	return( 3 );
    }

    for( long i = 0; i < kindex; i++)
	if( knotlist[i] > knotlist[i+1] ) {
	    // decreasing knot sequence
	    return( 4 );
	}
	
    /* check for valid multiplicity */

    /*	kindex is currently the index of the last knot.
     *	In the next loop  it is decremented to ignore the last knot
     *	and the loop stops when kindex  is 2 so as to ignore the first
     *  knot as well.  These knots are not used in computing 
     *  knot multiplicities.
     */

    long multi = 1;
    for( ; kindex >= 1; kindex-- ) {
	if( knotlist[kindex] - knotlist[kindex-1] < TOLERANCE ) {
	    multi++; 
	    continue;
	} 
	if ( multi > order ) {
            // knot multiplicity greater than order of spline
	    return( 5 );
	} 
	multi = 1;
    }

    if ( multi > order ) {
        // knot multiplicity greater than order of spline
	return( 5 );
    } 

    return 0;
}

void Knotvector::show( char *msg )
{
#ifndef NDEBUG
    dprintf( "%s\n", msg ); 
    dprintf( "order = %ld, count = %ld\n", order, knotcount );

    for( int i=0; i<knotcount; i++ )
	dprintf( "knot[%d] = %g\n", i, knotlist[i] );
#endif
}

