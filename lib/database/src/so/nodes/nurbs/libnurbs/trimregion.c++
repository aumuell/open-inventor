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
 * trimregion.c++ - $Revision: 1.2 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "trimregion.h"
#include "backend.h"

TrimRegion::TrimRegion( void )
{
}

void
TrimRegion::setDu( REAL du )
{
    oneOverDu = 1.0/du;
}

void
TrimRegion::init( long npts, Arc_ptr extrema )
{
    left.init( npts, extrema, extrema->pwlArc->npts - 1 ); 
    left.getNextPt();

    right.init( npts, extrema, 0 ); 
    right.getPrevPt();
}

void
TrimRegion::getPts( Arc_ptr extrema )
{
    left.getNextPts( extrema );
    right.getPrevPts( extrema );
}

void
TrimRegion::getPts( Backend &backend )
{
    left.getNextPts( bot.vval, backend );
    right.getPrevPts( bot.vval, backend );
}

void 
TrimRegion::getGridExtent( void )
{
    getGridExtent( left.last(), right.last() );
}

void
TrimRegion::getGridExtent( TrimVertex *l, TrimVertex *r )
{
    bot.ustart = (long) ((l->param[0] - uarray.uarray[0])*oneOverDu);
    if( l->param[0] >= uarray.uarray[bot.ustart] ) bot.ustart++;
//  if( l->param[0] > uarray.uarray[bot.ustart] ) bot.ustart++;
    assert( l->param[0] <= uarray.uarray[bot.ustart] );
    assert( l->param[0] >= uarray.uarray[bot.ustart-1] );

    bot.uend = (long) ((r->param[0] - uarray.uarray[0])*oneOverDu);
    if( uarray.uarray[bot.uend] >= r->param[0] ) bot.uend--;
//  if( uarray.uarray[bot.uend] > r->param[0] ) bot.uend--;
    assert( r->param[0] >= uarray.uarray[bot.uend] );
    assert( r->param[0] <= uarray.uarray[bot.uend+1] );
}

int
TrimRegion::canTile( void )
{
    TrimVertex *lf = left.first();
    TrimVertex *ll = left.last();
    TrimVertex *l = ( ll->param[0] > lf->param[0] ) ? ll : lf; 

    TrimVertex *rf = right.first();
    TrimVertex *rl = right.last();
    TrimVertex *r = ( rl->param[0] < rf->param[0] ) ? rl : rf;
    return (l->param[0] <= r->param[0]) ? 1 : 0;
}

