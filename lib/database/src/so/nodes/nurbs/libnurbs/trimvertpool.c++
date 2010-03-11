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
 * trimvertexpool.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "mystring.h"
#include "trimvertex.h"
#include "trimvertpool.h"
#include "bufpool.h"

/*----------------------------------------------------------------------------
 * TrimVertexPool::TrimVertexPool 
 *----------------------------------------------------------------------------
 */
TrimVertexPool::TrimVertexPool( void )
	: pool( sizeof(TrimVertex)*3, 32, "Threevertspool" )
{
    // initialize array of pointers to vertex lists
    nextvlistslot = 0;
    vlistsize = INIT_VERTLISTSIZE;
    vlist = new TrimVertex_p[vlistsize];
}

/*----------------------------------------------------------------------------
 * TrimVertexPool::~TrimVertexPool 
 *----------------------------------------------------------------------------
 */
TrimVertexPool::~TrimVertexPool( void )
{
    // free all arrays of TrimVertices vertices
    while( nextvlistslot ) {
	delete vlist[--nextvlistslot];
    }

    // reallocate space for array of pointers to vertex lists
    if( vlist ) delete[] vlist;
}

/*----------------------------------------------------------------------------
 * TrimVertexPool::clear 
 *----------------------------------------------------------------------------
 */
void
TrimVertexPool::clear( void )
{
    // reinitialize pool of 3 vertex arrays    
    pool.clear();

    // free all arrays of TrimVertices vertices
    while( nextvlistslot ) {
	delete vlist[--nextvlistslot];
	vlist[nextvlistslot] = 0;
    }

    // reallocate space for array of pointers to vertex lists
    if( vlist ) delete[] vlist;
    vlist = new TrimVertex_p[vlistsize];
}


/*----------------------------------------------------------------------------
 * TrimVertexPool::get - allocate a vertex list
 *----------------------------------------------------------------------------
 */
TrimVertex *
TrimVertexPool::get( int n )
{
    TrimVertex	*v;
    if( n == 3 ) {
	v = (TrimVertex *) pool.new_buffer();
    } else {
        if( nextvlistslot == vlistsize ) {
	    vlistsize *= 2;
	    TrimVertex_p *nvlist = new TrimVertex_p[vlistsize];
	    memcpy( nvlist, vlist, nextvlistslot * sizeof(TrimVertex_p) );
	    if( vlist ) delete[] vlist;
	    vlist = nvlist;
        }
        v = vlist[nextvlistslot++] = new TrimVertex[n];
    }
    return v;
}
