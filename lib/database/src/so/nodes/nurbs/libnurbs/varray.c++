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
 * varray.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "varray.h"
#include "arc.h"
#include "math.h"         // fabs()

#define TINY 0.0001
inline long sgn( REAL x ) 
{
    return (x < -TINY) ? -1 :  ((x > TINY) ? 1 : 0 );
}


Varray::Varray( void )
{
    varray = 0;
    size = 0;
}

Varray::~Varray( void )
{
    if( varray ) delete[] varray; 
}

inline void
Varray::update( Arc_ptr arc, long dir[2], REAL val )
{
    register long ds = sgn(arc->tail()[0] - arc->prev->tail()[0]);
    register long dt = sgn(arc->tail()[1] - arc->prev->tail()[1]);

    if( dir[0] != ds || dir[1] != dt ) {
	dir[0] = ds;
	dir[1] = dt;
	append( val );
    }
}

void
Varray::grow( long guess )
{
    if( size < guess ) {
	size = guess * 2;
	if( varray ) delete[] varray; 
	varray = new REAL[size];
	assert( varray );
    }
}

long
Varray::init( REAL delta, Arc_ptr toparc, Arc_ptr botarc )
{
    Arc_ptr left = toparc->next;
    Arc_ptr right = toparc;
    long ldir[2], rdir[2];
    
    ldir[0] = sgn( left->tail()[0] - left->prev->tail()[0] );
    ldir[1] = sgn( left->tail()[1] - left->prev->tail()[1] );
    rdir[0] = sgn( right->tail()[0] - right->prev->tail()[0] );
    rdir[1] = sgn( right->tail()[1] - right->prev->tail()[1] );

    vval[0] = toparc->tail()[1];
    numquads = 0;

    while( 1 ) {
	switch( sgn( left->tail()[1] - right->prev->tail()[1] ) ) {
	case 1:
	    left = left->next;
	    update( left, ldir, left->prev->tail()[1] );
	    break;
	case -1: 
	    right = right->prev;
	    update( right, rdir, right->tail()[1] );
	    break;
	case 0:
	    if( fabs(left->tail()[1] - botarc->tail()[1]) < TINY) goto end;
            if( fabs(left->tail()[0]-right->prev->tail()[0]) < TINY &&
                fabs(left->tail()[1]-right->prev->tail()[1]) < TINY) goto end;
	    left = left->next;
	    break;
 	}
    }

end:
    append( botarc->tail()[1] );

    grow( ((long) ((vval[0] - vval[numquads])/delta)) + numquads + 2 );

    long i, index = 0;
    for( i=0; i<numquads; i++ ) {
	voffset[i] = index;
        varray[index++] = vval[i];
	REAL dist = vval[i] - vval[i+1];
	if( dist > delta ) {
	    long steps = ((long) (dist/delta)) +1;
	    float deltav = - dist / (REAL) steps;
	    for( long j=1; j<steps; j++ ) 
		varray[index++] = vval[i] + j * deltav; 
	}
    }
    voffset[i] = index;
    varray[index] = vval[i];
    return index;
}

