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

#ifndef __glupatch_h_
#define __glupatch_h_

#include "../SoAddPrefix.h"
/*
 * patch.h - $Revision: 1.1.1.1 $
 */

#include "types.h"
#include "defines.h"

class Quilt;
class Mapdesc;


struct Pspec {
    REAL		range[3];
    REAL		sidestep[2];
    REAL		stepsize;
    REAL		minstepsize;
    int			needsSubdivision;
};

struct Patchspec : public Pspec {
    int			order;
    int			stride;
    void 		clamp( REAL );
    void 		getstepsize( REAL );
    void		singleStep( void );
};

class Patch {
public:
friend class Subdivider;
friend class Quilt;
friend class Patchlist;
    			Patch( Quilt *, REAL*, REAL *, Patch * );
    			Patch( Patch &, int, REAL, Patch * );
    void		bbox( void );
    void		clamp( void );
    void		getstepsize( void );
    int			cullCheck( void );
    int			needsSubdivision( int );
    int			needsSamplingSubdivision( void );
    int			needsNonSamplingSubdivision( void );

private:

    Mapdesc*		mapdesc;
    Patch*		next;
    int			cullval;
    int			notInBbox;
    int			needsSampling;
    REAL		cpts[MAXORDER*MAXORDER*MAXCOORDS]; //culling pts 
    REAL		spts[MAXORDER*MAXORDER*MAXCOORDS]; //sampling pts 
    REAL		bpts[MAXORDER*MAXORDER*MAXCOORDS]; //bbox pts
    Patchspec		pspec[2];
    void 		checkBboxConstraint( void );
    REAL 		bb[2][MAXCOORDS];
};
#endif /* __glupatch_h_ */
