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
 * maplist.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mystdio.h"
#include "myassert.h"
#include "mymath.h"
#include "nurbsconsts.h"
#include "maplist.h"
#include "mapdesc.h"
#include "backend.h"
 
Maplist::Maplist( Backend& b )
    : mapdescPool( sizeof( Mapdesc ), 10, "mapdesc pool" ),
      backend( b )
{
    maps = 0; lastmap = &maps;
}

void 
Maplist::initialize( void )
{
    freeMaps();
    define( N_P2D, 0, 2 );
    define( N_P2DR, 1, 3 );
}

void 
Maplist::add( long type, int israt, int ncoords )
{
    *lastmap = new(mapdescPool) Mapdesc( type, israt, ncoords, backend );
    lastmap = &((*lastmap)->next);
}

void 
Maplist::define( long type, int israt, int ncoords )
{
    Mapdesc *m = locate( type );
    assert( m == NULL || ( m->isrational == israt && m->ncoords == ncoords ) );
    add( type, israt, ncoords );
}

void 
Maplist::remove( Mapdesc *m )
{
    for( Mapdesc **curmap = &maps; *curmap; curmap = &((*curmap)->next) ) {
	if( *curmap == m ) {
	    *curmap = m->next;
	    m->deleteMe( mapdescPool );
	    return;
	}
    }
    abort();
}

void
Maplist::freeMaps( void )
{
    mapdescPool.clear();
    maps = 0;
    lastmap = &maps;
}

Mapdesc * 
Maplist::find( long type )
{
    Mapdesc *val = locate( type );
    assert( val );
    return val;
}

Mapdesc * 
Maplist::locate( long type )
{
    Mapdesc *m;
    for( m = maps; m; m = m->next )
	if( m->getType() == type ) break;
    return m;
}
