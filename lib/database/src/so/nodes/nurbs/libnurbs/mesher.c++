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
 * mesher.c++ - $Revision: 1.2 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "mesher.h"
#include "gridvertex.h"
#include "gridtrimvertex.h"
#include "jarcloc.h"
#include "gridline.h"
#include "trimline.h"
#include "uarray.h"
#include "backend.h"


const float Mesher::ZERO = 0.0;

Mesher::Mesher( Backend& b ) 
	: backend( b ), 
	p( sizeof( GridTrimVertex ), 100, "GridTrimVertexPool" )
{
    stacksize = 0;
    vdata = 0;
    lastedge = 0;
}

Mesher::~Mesher( void )
{
    if( vdata ) delete[] vdata;
}

void 
Mesher::init( unsigned int npts )
{
    p.clear();
    if( stacksize < npts ) {
	stacksize = 2 * npts;
	if( vdata ) delete[] vdata;		
	vdata = new GridTrimVertex_p[stacksize];
    } 
}

inline void
Mesher::push( GridTrimVertex *gt )
{
    assert( itop+1 != stacksize );
    vdata[++itop] = gt;
}

inline void
Mesher::pop( long )
{
}

inline void
Mesher::openMesh()
{
    backend.bgntmesh( "addedge" );
}

inline void
Mesher::closeMesh()
{
    backend.endtmesh();
}

inline void
Mesher::swapMesh()
{
    backend.swaptmesh();
}

inline void
Mesher::clearStack()
{
    itop = -1;
    last[0] = 0;
}

void
Mesher::finishLower( GridTrimVertex *gtlower )
{
    for( push(gtlower); 
	 nextlower( gtlower=new(p) GridTrimVertex ); 
	 push(gtlower) ) 
	    addLower();
    addLast();
}

void
Mesher::finishUpper( GridTrimVertex *gtupper )
{
    for( push(gtupper); 
	 nextupper( gtupper=new(p) GridTrimVertex ); 
	 push(gtupper) ) 
	    addUpper();
    addLast();
}

void
Mesher::mesh( void )
{
    GridTrimVertex *gtlower, *gtupper;

    Hull::init( );
    nextupper( gtupper = new(p) GridTrimVertex );
    nextlower( gtlower = new(p) GridTrimVertex );

    clearStack();
    openMesh();
    push(gtupper);

    nextupper( gtupper = new(p) GridTrimVertex );
    nextlower( gtlower );

    assert( gtupper->t && gtlower->t );
    
    if( gtupper->t->param[0] < gtlower->t->param[0] ) {
	push(gtupper);
	lastedge = 1;
	if( nextupper( gtupper=new(p) GridTrimVertex ) == 0 ) {
	    finishLower(gtlower);
	    return;
	}
    } else if( gtupper->t->param[0] > gtlower->t->param[0] ) {
	push(gtlower);
	lastedge = 0;
	if( nextlower( gtlower=new(p) GridTrimVertex ) == 0 ) {
	    finishUpper(gtupper);
	    return;
	}
    } else {
	if( lastedge == 0 ) {
	    push(gtupper);
	    lastedge = 1;
	    if( nextupper(gtupper=new(p) GridTrimVertex) == 0 ) {
		finishLower(gtlower);
		return;
	    }
	} else {
	    push(gtlower);
	    lastedge = 0;
	    if( nextlower( gtlower=new(p) GridTrimVertex ) == 0 ) {
		finishUpper(gtupper);
		return;
	    }
	}
    }

    while ( 1 ) {
	if( gtupper->t->param[0] < gtlower->t->param[0] ) {
            push(gtupper);
	    addUpper();
	    if( nextupper( gtupper=new(p) GridTrimVertex ) == 0 ) {
		finishLower(gtlower);
		return;
	    }
	} else if( gtupper->t->param[0] > gtlower->t->param[0] ) {
    	    push(gtlower);
	    addLower();
	    if( nextlower( gtlower=new(p) GridTrimVertex ) == 0 ) {
		finishUpper(gtupper);
		return;
	    }
	} else {
	    if( lastedge == 0 ) {
		push(gtupper);
		addUpper();
		if( nextupper( gtupper=new(p) GridTrimVertex ) == 0 ) {
		    finishLower(gtlower);
		    return;
		}
	    } else {
		push(gtlower);
		addLower();
		if( nextlower( gtlower=new(p) GridTrimVertex ) == 0 ) {
		    finishUpper(gtupper);
		    return;
		}
	    }
	}
    }
}

inline int
Mesher::isCcw( int ilast )
{
    REAL area = det3( vdata[ilast]->t, vdata[itop-1]->t, vdata[itop-2]->t );
    return (area < ZERO) ? 0 : 1;
}

inline int
Mesher::isCw( int ilast  )
{
    REAL area = det3( vdata[ilast]->t, vdata[itop-1]->t, vdata[itop-2]->t );
    return (area > -ZERO) ? 0 : 1;
}

inline int
Mesher::equal( int x, int y )
{
    return( last[0] == vdata[x] && last[1] == vdata[y] );
}

inline void
Mesher::copy( int x, int y )
{
    last[0] = vdata[x]; last[1] = vdata[y];
}
 
inline void
Mesher::move( int x, int y ) 
{
    vdata[x] = vdata[y];
}

inline void
Mesher::output( int x )
{
    backend.tmeshvert( vdata[x] );
}

/*---------------------------------------------------------------------------
 * addedge - addedge an edge to the triangulation
 *
 *	This code has been re-written to generate large triangle meshes
 *	from a monotone polygon.  Although smaller triangle meshes
 *	could be generated faster and with less code, larger meshes
 *	actually give better SYSTEM performance.  This is because
 *	vertices are processed in the backend slower than they are
 *	generated by this code and any decrease in the number of vertices
 *	results in a decrease in the time spent in the backend.
 *---------------------------------------------------------------------------
 */

void
Mesher::addLast( )
{
    register int ilast = itop;

    if( lastedge == 0 ) {
	if( equal( 0, 1 ) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i = 2; i < ilast; i++ ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, ilast-1 );
	} else if( equal( ilast-2, ilast-1) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i = ilast-3; i >= 0; i-- ) {
		output( i );
		swapMesh();
	    }
	    copy( 0, ilast );
	} else {
	    closeMesh();	openMesh();
	    output( ilast );
	    output( 0 );
	    for( register int i = 1; i < ilast; i++ ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, ilast-1 );
	}
    } else {
	if( equal( 1, 0) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i = 2; i < ilast; i++ ) {
		output( i );
		swapMesh();
	    }
	    copy( ilast-1, ilast );
	} else if( equal( ilast-1, ilast-2) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i = ilast-3; i >= 0; i-- ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, 0 );
	} else {
	    closeMesh();	openMesh();
	    output( 0 );
	    output( ilast );
	    for( register int i = 1; i < ilast; i++ ) {
		output( i );
		swapMesh();
	    }
	    copy( ilast-1, ilast );
	}
    }
    closeMesh();
    //for( register long k=0; k<=ilast; k++ ) pop( k );
}

void
Mesher::addUpper( )
{
    register int ilast = itop;

    if( lastedge == 0 ) {
	if( equal( 0, 1 ) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i = 2; i < ilast; i++ ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, ilast-1 );
	} else if( equal( ilast-2, ilast-1) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i = ilast-3; i >= 0; i-- ) {
		output( i );
		swapMesh();
	    }
	    copy( 0, ilast );
	} else {
	    closeMesh();	openMesh();
	    output( ilast );
	    output( 0 );
	    for( register int i = 1; i < ilast; i++ ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, ilast-1 );
	}
	lastedge = 1;
        //for( register long k=0; k<ilast-1; k++ ) pop( k );
	move( 0, ilast-1 );
	move( 1, ilast );
	itop = 1;
    } else {
	if( ! isCcw( ilast ) ) return;
	do {
	    itop--;
	} while( (itop > 1) && isCcw( ilast ) );

	if( equal( ilast-1, ilast-2 ) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i=ilast-3; i>=itop-1; i-- ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, itop-1 );
	} else if( equal( itop, itop-1 ) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i = itop+1; i < ilast; i++ ) {
		output( i );
		swapMesh();
	    }
	    copy( ilast-1, ilast );
	} else {
	    closeMesh();	openMesh();
	    output( ilast );
	    output( ilast-1 );
	    for( register int i=ilast-2; i>=itop-1; i-- ) {
		swapMesh();
		output( i );
	    } 
	    copy( ilast, itop-1 );
	}
        //for( register int k=itop; k<ilast; k++ ) pop( k );
	move( itop, ilast );
    }
}

void
Mesher::addLower()
{
    register int ilast = itop;

    if( lastedge == 1 ) {
	if( equal( 1, 0) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i = 2; i < ilast; i++ ) {
		output( i );
		swapMesh();
	    }
	    copy( ilast-1, ilast );
	} else if( equal( ilast-1, ilast-2) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i = ilast-3; i >= 0; i-- ) {
		swapMesh();
		output( i );
	    }
	    copy( ilast, 0 );
	} else {
	    closeMesh();	openMesh();
	    output( 0 );
	    output( ilast );
	    for( register int i = 1; i < ilast; i++ ) {
		output( i );
		swapMesh();
	    }
	    copy( ilast-1, ilast );
	}

	lastedge = 0;
        //for( register long k=0; k<ilast-1; k++ ) pop( k );
	move( 0, ilast-1 );
	move( 1, ilast );
	itop = 1;
    } else {
	if( ! isCw( ilast ) ) return;
	do {
	    itop--;
	} while( (itop > 1) && isCw( ilast ) );

	if( equal( ilast-2, ilast-1) ) {
	    swapMesh();
	    output( ilast );
	    for( register int i=ilast-3; i>=itop-1; i--) {
		output( i );
		swapMesh( );
	    }
	    copy( itop-1, ilast );
	} else if( equal( itop-1, itop) ) {
	    output( ilast );
	    swapMesh();
	    for( register int i=itop+1; i<ilast; i++ ) {
		swapMesh( );
		output( i );
	    }
	    copy( ilast, ilast-1 );
	} else {
	    closeMesh();	openMesh();
	    output( ilast-1 );
	    output( ilast );
	    for( register int i=ilast-2; i>=itop-1; i-- ) {
		output( i );
		swapMesh( );
	    }
	    copy( itop-1, ilast );
	}
        //for( register int k=itop; k<ilast; k++ ) pop( k );
	move( itop, ilast );
    }
}


