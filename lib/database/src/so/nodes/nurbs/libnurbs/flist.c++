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
 * flist.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "flist.h"

/*----------------------------------------------------------------------------
 * Flist::Flist - initialize a REAL number array
 *----------------------------------------------------------------------------
 */
Flist::Flist( void )
{
    npts = 0;
    pts = 0;
    start = end = 0;
}

/*----------------------------------------------------------------------------
 * Flist::~Flist - free a REAL number array
 *----------------------------------------------------------------------------
 */
Flist::~Flist( void )
{
    if( npts ) delete[] pts;
}

void
Flist::add( REAL x )
{
    pts[end++] = x;
    assert( end <= npts );
}

/*----------------------------------------------------------------------------
 * Flist::filter - remove duplicate numbers from array
 *----------------------------------------------------------------------------
 */
void Flist::filter( void )
{
    sorter.qsort( pts, end );
    start = 0;

    int j = 0;
    for( int i = 1; i < end; i++ ) { 
	if( pts[i] == pts[i-j-1] )
	    j++;
	pts[i-j] = pts[i];
    }
    end -= j;
}

/*----------------------------------------------------------------------------
 * Flist::grow - ensure that array is large enough
 *----------------------------------------------------------------------------
 */
void Flist::grow( int maxpts )
{
    if( npts < maxpts ) {
	if( npts ) delete[] pts;
	npts = 2 * maxpts; 
	pts = new REAL[npts];
	assert( pts );
    }
    start = end = 0;
}

/*----------------------------------------------------------------------------
 * Flist::taper - ignore head and tail of array
 *----------------------------------------------------------------------------
 */
void Flist::taper( REAL from, REAL to )
{
    while( pts[start] != from )
	start++;

    while( pts[end-1] != to )
	end--;
}


