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
 * bin.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mystdio.h"
#include "myassert.h"
#include "bin.h"

/*----------------------------------------------------------------------------
 * Constructor and destructor
 *----------------------------------------------------------------------------
 */
Bin::Bin()
{
    head = NULL;
}

Bin::~Bin()
{
    assert( head == NULL);
}

/*----------------------------------------------------------------------------
 * remove_this_arc - remove given Arc_ptr from bin
 *----------------------------------------------------------------------------
 */

void 
Bin::remove_this_arc( Arc_ptr arc )
{
    Arc_ptr *j;
    for( j = &(head); (*j != 0) && (*j != arc); j = &((*j)->link) );

    if( *j != 0 ) {
        if( *j == current )
	    current = (*j)->link;
	*j = (*j)->link;
    }
}

/*----------------------------------------------------------------------------
 * numarcs - count number of arcs in bin
 *----------------------------------------------------------------------------
 */

int
Bin::numarcs()
{
    long count = 0;
    for( Arc_ptr jarc = firstarc(); jarc; jarc = nextarc() )
	count++;
    return count;
}

/*----------------------------------------------------------------------------
 * adopt - place an orphaned arcs into their new parents bin
 *----------------------------------------------------------------------------
 */

void 
Bin::adopt()
{
    markall();

    Arc_ptr orphan;
    while( orphan = removearc() ) {
	for( Arc_ptr parent = orphan->next; parent != orphan; parent = parent->next ) {
	    if (! parent->ismarked() ) {
		orphan->link = parent->link;
		parent->link = orphan;
		orphan->clearmark();
		break;
	    }
	}
    }
}


/*----------------------------------------------------------------------------
 * show - print out descriptions of the arcs in the bin
 *----------------------------------------------------------------------------
 */

void
Bin::show( char *name )
{
#ifndef NDEBUG
    dprintf( "%s\n", name );
    for( Arc_ptr jarc = firstarc(); jarc; jarc = nextarc() )
        jarc->show( );
#endif
}



/*----------------------------------------------------------------------------
 * markall - mark all arcs with an identifying tag
 *----------------------------------------------------------------------------
 */

void 
Bin::markall()
{
    for( Arc_ptr jarc=firstarc(); jarc; jarc=nextarc() )
	jarc->setmark();
}

/*----------------------------------------------------------------------------
 * listBezier - print out all arcs that are untessellated border arcs
 *----------------------------------------------------------------------------
 */

void 
Bin::listBezier( void )
{
    for( Arc_ptr jarc=firstarc(); jarc; jarc=nextarc() ) {
	if( jarc->isbezier( ) ) {
    	    assert( jarc->pwlArc->npts == 2 );	
	    TrimVertex  *pts = jarc->pwlArc->pts;
    	    REAL s1 = pts[0].param[0];
    	    REAL t1 = pts[0].param[1];
    	    REAL s2 = pts[1].param[0];
    	    REAL t2 = pts[1].param[1];
#ifndef NDEBUG
	   dprintf( "arc (%g,%g) (%g,%g)\n", s1, t1, s2, t2 );
#endif
	}
    }
}

