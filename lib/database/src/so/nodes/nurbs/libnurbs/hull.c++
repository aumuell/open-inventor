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
 * hull.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "hull.h"
#include "gridvertex.h"
#include "gridtrimvertex.h"
#include "gridline.h"
#include "trimline.h"
#include "uarray.h"
#include "trimregion.h"

Hull::Hull( void )
{}

Hull::~Hull( void )
{}

/*----------------------------------------------------------------------
 * Hull:init - this routine does the initialization needed before any
 *	 	calls to nextupper or nextlower can be made.
 *----------------------------------------------------------------------
 */
void
Hull::init( void )
{
    TrimVertex *lfirst = left.first();
    TrimVertex *llast = left.last();
    if( lfirst->param[0] <= llast->param[0] ) {
	fakeleft.init( left.first() );
	upper.left = &fakeleft;
	lower.left = &left;
    } else {
	fakeleft.init( left.last() );
	lower.left = &fakeleft;
 	upper.left = &left;
    }
    upper.left->last();
    lower.left->first();

    if( top.ustart <= top.uend ) {
	upper.line = &top;
	upper.index = top.ustart;
    } else
	upper.line = 0;

    if( bot.ustart <= bot.uend ) {
	lower.line = &bot;
	lower.index = bot.ustart;
    } else
	lower.line = 0;

    TrimVertex *rfirst = right.first();
    TrimVertex *rlast = right.last();
    if( rfirst->param[0] <= rlast->param[0] ) {
	fakeright.init( right.last() );
	lower.right = &fakeright;
	upper.right = &right;
    } else {
	fakeright.init( right.first() );
	upper.right = &fakeright;
	lower.right = &right;
    }
    upper.right->first();
    lower.right->last();
}

/*----------------------------------------------------------------------
 * nextupper - find next vertex on upper hull of trim region.
 *		 - if vertex is on trim curve, set vtop point to 
 *		   that vertex.  if vertex is on grid, set vtop to
 *		   point to temporary area and stuff coordinants into
 *		   temporary vertex.  Also, place grid coords in temporary
 *		   grid vertex.
 *----------------------------------------------------------------------
 */
GridTrimVertex *
Hull::nextupper( GridTrimVertex *gv )
{
    if( upper.left ) {
	gv->set( upper.left->prev() );
	if( gv->isTrimVert() ) return gv;
	upper.left = 0;
    } 

    if( upper.line ) {
	assert( upper.index <= upper.line->uend );
	gv->set( uarray.uarray[upper.index], upper.line->vval );
	gv->set( upper.index, upper.line->vindex );
	if( upper.index++ == upper.line->uend ) upper.line = 0;
	return gv; 
    } 

    if( upper.right ) {
	gv->set( upper.right->next() );
	if( gv->isTrimVert() ) return gv;
	upper.right = 0;
    } 

    return 0; 
}

GridTrimVertex *
Hull::nextlower( register GridTrimVertex *gv )
{
    if( lower.left ) {
	gv->set( lower.left->next() );
	if( gv->isTrimVert() ) return gv;
	lower.left = 0;
    } 

    if( lower.line ) {
	gv->set( uarray.uarray[lower.index], lower.line->vval );
	gv->set( lower.index, lower.line->vindex );
	if( lower.index++ == lower.line->uend ) lower.line = 0;
	return gv;
    } 

    if( lower.right ) {
	gv->set( lower.right->prev() );
	if( gv->isTrimVert() ) return gv;
	lower.right = 0;
    } 

    return 0;
}

