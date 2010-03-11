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
 * arc.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */


#include "glimports.h"
#include "mystdio.h"
#include "myassert.h"
#include "arc.h"
#include "bin.h"
#include "bezierarc.h"
#include "pwlarc.h"
#include "simplemath.h"

/* local preprocessor definitions */
#define	ZERO		0.000001

const int 	Arc::bezier_tag = (1<<13);
const int 	Arc::arc_tag = (1<<3);
const int 	Arc::tail_tag = (1<<6);

/*--------------------------------------------------------------------------
 * makeSide - attach a pwl arc to an arc and mark it as a border arc
 *--------------------------------------------------------------------------
 */

void
Arc::makeSide( PwlArc *pwl, arc_side side )
{
    assert( pwl );
    assert( pwlArc == 0 );
    assert( pwl->npts > 0 );
    assert( pwl->pts );
    pwlArc = pwl;
    clearbezier();
    setside( side );
}
 

/*--------------------------------------------------------------------------
 * numpts - count number of points on arc loop
 *--------------------------------------------------------------------------
 */

int
Arc::numpts( void )
{
    Arc_ptr jarc = this;
    int npts = 0;
    do {
	npts += jarc->pwlArc->npts;
	jarc = jarc->next;
    } while( jarc != this );
    return npts;
}

/*--------------------------------------------------------------------------
 * markverts - mark each point with id of arc
 *--------------------------------------------------------------------------
 */

void
Arc::markverts( void )
{
    Arc_ptr jarc = this;
	
    do {
	TrimVertex *p = jarc->pwlArc->pts;
	for( int i=0; i<jarc->pwlArc->npts; i++ )
	    p[i].nuid = jarc->nuid;
	jarc = jarc->next;
    } while( jarc != this );
}

/*--------------------------------------------------------------------------
 * getextrema - find axis extrema on arc loop
 *--------------------------------------------------------------------------
 */

void
Arc::getextrema( Arc_ptr extrema[4] )
{
    REAL leftpt, botpt, rightpt, toppt;

    extrema[0] = extrema[1] = extrema[2] = extrema[3] = this;

    leftpt = rightpt = this->tail()[0];
    botpt  = toppt   = this->tail()[1];

    for( Arc_ptr jarc = this->next; jarc != this; jarc = jarc->next ) {
	if ( jarc->tail()[0] <  leftpt || 
	    (jarc->tail()[0] <= leftpt && jarc->rhead()[0]<=leftpt))  {
	    leftpt = jarc->pwlArc->pts->param[0];
	    extrema[1] = jarc;
	}
	if ( jarc->tail()[0] >  rightpt || 
	    (jarc->tail()[0] >= rightpt && jarc->rhead()[0] >= rightpt)) {
	    rightpt = jarc->pwlArc->pts->param[0];
	    extrema[3] = jarc;
	}
	if ( jarc->tail()[1] <  botpt || 
            (jarc->tail()[1] <= botpt && jarc->rhead()[1] <= botpt ))  {
	    botpt = jarc->pwlArc->pts->param[1];
	    extrema[2] = jarc;
	}
	if ( jarc->tail()[1] >  toppt || 
	    (jarc->tail()[1] >= toppt && jarc->rhead()[1] >= toppt))  {
	    toppt = jarc->pwlArc->pts->param[1];
	    extrema[0] = jarc;
	}
    }
}


/*-------------------------------------------------------------------------
 * show - print to the stdout the vertices of a pwl arc
 *-------------------------------------------------------------------------
 */

void
Arc::show()
{
/*#ifndef NDEBUG*/
     printf( "\tPWLARC NP: %d FL: 1\n", pwlArc->npts );
    for( int i = 0; i < pwlArc->npts; i++ ) {
          printf( "\t\tVERTEX %f %f\n", pwlArc->pts[i].param[0],
			pwlArc->pts[i].param[1] );
    }
/*#endif*/
}

/*-------------------------------------------------------------------------
 * print - print out the vertices of all pwl arcs on a loop
 *-------------------------------------------------------------------------
 */

void
Arc::print( void )
{
    Arc_ptr jarc = this;

    if( ! this ) {
#ifndef NDEBUG
	dprintf( "\n\nEMPTY TRIM\n\n" );
#endif
	return;
    }

#ifndef NDEBUG
    dprintf( "BGNTRIM\n" );
#endif
    do {
	jarc->show( );
	jarc = jarc->next;
    } while (jarc != this);
#ifndef NDEBUG
    dprintf("ENDTRIM\n" );
#endif
}

/*-------------------------------------------------------------------------
 * isDisconnected - check if tail of arc and head of prev meet
 *-------------------------------------------------------------------------
 */

int
Arc::isDisconnected( void )
{
    if( pwlArc == 0 ) return 0;
    if( prev->pwlArc == 0 ) return 0;

    REAL *p0 = tail();
    REAL *p1 = prev->rhead();

    if( ((p0[0] - p1[0]) > ZERO) || ((p1[0] - p0[0]) > ZERO) ||
	((p0[1] - p1[1]) > ZERO) || ((p1[1] - p0[1]) > ZERO)  ) {
#ifndef NDEBUG
	dprintf( "x coord = %f %f %f\n", p0[0], p1[0], p0[0] - p1[0] );
	dprintf( "y coord = %f %f %f\n", p0[1], p1[1], p0[1] - p1[1] );
#endif
	return 1;
    } else {
        /* average two points together */
        p0[0] = p1[0] = (p1[0] + p0[0]) * 0.5;
        p0[1] = p1[1] = (p1[1] + p0[1]) * 0.5;
        return 0;
    }
}

/*-------------------------------------------------------------------------
 * neq_vert - assert that two 2D vertices are not equal
 *-------------------------------------------------------------------------
 */

inline static int
neq_vert( REAL	*v1, REAL *v2 )
{
     return ((v1[0] != v2[0]) || (v1[1] != v2[1] )) ? 1 : 0;
}

/*-------------------------------------------------------------------------
 * check - verify consistency of a loop, including
 *		1) if pwl, no two consecutive vertices are identical
 *		2) the circular link pointers are valid
 *		3) the geometric info at the head and tail are consistent
 *-------------------------------------------------------------------------
 */

int
Arc::check( void )
{
    if( this == 0 ) return 1;
    Arc_ptr jarc = this;
    do {
	assert( jarc->pwlArc || jarc->bezierArc );

        if (jarc->prev == 0 || jarc->next == 0) {
#ifndef NDEBUG
	    dprintf( "checkjarc:null next/prev pointer\n");
	    jarc->print( );
#endif
	    return 0;
        }

        if (jarc->next->prev != jarc) {
#ifndef NDEBUG
	    dprintf( "checkjarc: pointer linkage screwed up\n");
	    jarc->print( );
#endif
	    return 0;
        }

        if( jarc->pwlArc ) {
#ifndef NDEBUG
	    assert( jarc->pwlArc->npts > 1 );
	    assert( jarc->pwlArc->npts < 100000 );
/*
	    for( int i=0; i < jarc->pwlArc->npts-1; i++ )
		assert( neq_vert( jarc->pwlArc->pts[i].param, 
			     jarc->pwlArc->pts[i+1].param) );
*/
#endif
	    if( jarc->prev->pwlArc ) {
		if( jarc->tail()[1] != jarc->prev->rhead()[1] ) {
#ifndef NDEBUG
		    dprintf( "checkjarc: geometric linkage screwed up 1\n");
		    jarc->prev->show();
		    jarc->show();
#endif
		    return 0;
		}
		if( jarc->tail()[0] != jarc->prev->rhead()[0] ) {
		    
#ifndef NDEBUG
		    dprintf( "checkjarc: geometric linkage screwed up 2\n");
		    jarc->prev->show();
		    jarc->show();
#endif
		    return 0;
		}
	    }
	    if( jarc->next->pwlArc ) {
		if( jarc->next->tail()[0] != jarc->rhead()[0] ) {
#ifndef NDEBUG
			dprintf( "checkjarc: geometric linkage screwed up 3\n");
			jarc->show();
			jarc->next->show();
#endif
			return 0;
		}
		if( jarc->next->tail()[1] != jarc->rhead()[1] ) {
#ifndef NDEBUG
			dprintf( "checkjarc: geometric linkage screwed up 4\n");
			jarc->show();
			jarc->next->show();
#endif
			return 0;
		}
	    }
	    if( jarc->isbezier() ) {
		assert( jarc->pwlArc->npts == 2 );
    		assert( (jarc->pwlArc->pts[0].param[0] == \
	    		jarc->pwlArc->pts[1].param[0]) ||\
    	    		(jarc->pwlArc->pts[0].param[1] == \
	    		jarc->pwlArc->pts[1].param[1]) );
	    }
	}
        jarc = jarc->next;
    } while (jarc != this);
    return 1;
}


#define TOL 0.00001

inline long tooclose( REAL x, REAL y )
{
    return (abs(x-y) < TOL) ?  1 : 0;
}


/*--------------------------------------------------------------------------
 * append - append a jordan arc to a circularly linked list
 *--------------------------------------------------------------------------
 */

Arc_ptr
Arc::append( Arc_ptr jarc )
{
    if( jarc != 0 ) {
	next = jarc->next;
	prev = jarc;
   	next->prev = prev->next = this;
    } else {
        next = prev = this;
    }
    return this;
}


#undef	ZERO		/* be nice where Inventor #include's c++ files */
