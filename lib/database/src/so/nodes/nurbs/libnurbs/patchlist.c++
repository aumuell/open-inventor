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
 * patchlist.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */
#include <stdio.h>
#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "quilt.h"
#include "patchlist.h"
#include "patch.h"
#include "nurbsconsts.h"

Patchlist::Patchlist( Quilt *quilts, REAL *pta, REAL *ptb )
{
    patch = 0;
    for( Quilt *q = quilts; q; q = q->next ) 
	patch = new Patch( q, pta, ptb, patch );
    pspec[0].range[0] = pta[0];
    pspec[0].range[1] = ptb[0];
    pspec[0].range[2] = ptb[0] - pta[0];
 
    pspec[1].range[0] = pta[1];
    pspec[1].range[1] = ptb[1];
    pspec[1].range[2] = ptb[1] - pta[1];
}

Patchlist::Patchlist( Patchlist &upper, int param,  REAL value)
{
    Patchlist &lower = *this;
    patch = 0;
    for( Patch *p = upper.patch; p; p = p->next )
	patch = new Patch( *p, param, value, patch );

    if( param == 0 ) {
	lower.pspec[0].range[0] = upper.pspec[0].range[0];
	lower.pspec[0].range[1] = value;
	lower.pspec[0].range[2] = value - upper.pspec[0].range[0];
	upper.pspec[0].range[0] = value;
	upper.pspec[0].range[2] = upper.pspec[0].range[1] - value;
	lower.pspec[1] = upper.pspec[1];
    } else {
	lower.pspec[0] = upper.pspec[0];
	lower.pspec[1].range[0] = upper.pspec[1].range[0];
	lower.pspec[1].range[1] = value;
	lower.pspec[1].range[2] = value - upper.pspec[1].range[0];
	upper.pspec[1].range[0] = value;
	upper.pspec[1].range[2] = upper.pspec[1].range[1] - value;
    }
}

Patchlist::~Patchlist()
{
    while( patch ) {
	Patch *p = patch;
	patch = patch->next;
	delete p;
    }
}

int
Patchlist::cullCheck( void )
{
    for( Patch *p = patch; p; p = p->next )
	if( p->cullCheck() == CULL_TRIVIAL_REJECT )
	    return CULL_TRIVIAL_REJECT;
    return CULL_ACCEPT;
}

void
Patchlist::getstepsize( void )
{
    pspec[0].stepsize    = pspec[0].range[2];
    pspec[0].sidestep[0] = pspec[0].range[2];
    pspec[0].sidestep[1] = pspec[0].range[2];

    pspec[1].stepsize    = pspec[1].range[2];
    pspec[1].sidestep[0] = pspec[1].range[2];
    pspec[1].sidestep[1] = pspec[1].range[2];

    for( Patch *p = patch; p; p = p->next ) {
	p->getstepsize();
	p->clamp();
	pspec[0].stepsize    =  ((p->pspec[0].stepsize < pspec[0].stepsize) ? p->pspec[0].stepsize : pspec[0].stepsize);
	pspec[0].sidestep[0] =  ((p->pspec[0].sidestep[0] < pspec[0].sidestep[0]) ? p->pspec[0].sidestep[0] : pspec[0].sidestep[0]);
	pspec[0].sidestep[1] =  ((p->pspec[0].sidestep[1] < pspec[0].sidestep[1]) ? p->pspec[0].sidestep[1] : pspec[0].sidestep[1]);
	pspec[1].stepsize    =  ((p->pspec[1].stepsize < pspec[1].stepsize) ? p->pspec[1].stepsize : pspec[1].stepsize);
	pspec[1].sidestep[0] =  ((p->pspec[1].sidestep[0] < pspec[1].sidestep[0]) ? p->pspec[1].sidestep[0] : pspec[1].sidestep[0]);
	pspec[1].sidestep[1] =  ((p->pspec[1].sidestep[1] < pspec[1].sidestep[1]) ? p->pspec[1].sidestep[1] : pspec[1].sidestep[1]);
    }
}

void
Patchlist::bbox( void )
{
    for( Patch *p = patch; p; p = p->next )
	p->bbox();
}

int
Patchlist::needsNonSamplingSubdivision( void )
{
    notInBbox = 0;
    for( Patch *p = patch; p; p = p->next )
	notInBbox |= p->needsNonSamplingSubdivision();
    return notInBbox;
}

int
Patchlist::needsSamplingSubdivision( void )
{
    pspec[0].needsSubdivision = 0;
    pspec[1].needsSubdivision = 0;

    for( Patch *p = patch; p; p = p->next ) {
	pspec[0].needsSubdivision |= p->pspec[0].needsSubdivision;
	pspec[1].needsSubdivision |= p->pspec[0].needsSubdivision;
    }
    return (pspec[0].needsSubdivision || pspec[1].needsSubdivision) ? 1 : 0;
}

int
Patchlist::needsSubdivision( int param )
{
    return pspec[param].needsSubdivision;
}
