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

#ifndef __gluquilt_h_
#define __gluquilt_h_

#include "../SoAddPrefix.h"
/*
 * quilt.h - $Revision: 1.1 $
 */

#include "defines.h"
#include "bufpool.h"
#include "types.h"

class Backend;
class Mapdesc;
class Flist;
class Knotvector;

/* constants for memory allocation of NURBS to Bezier conversion */ 
#define	MAXDIM 		2

struct Quiltspec { /* a specification for a dimension of a quilt */
    int			stride;		/* words between points */
    int			width;		/* number of segments */
    int			offset;		/* words to first point */
    int			order;		/* order */
    int			index;		/* current segment number */
    int			bdry[2];	/* boundary edge flag */
    REAL  		step_size;
    Knot *		breakpoints;
};

typedef Quiltspec *Quiltspec_ptr;
    
struct Quilt : PooledObj { /* an array of bezier patches */
    			Quilt( Mapdesc * );
    Mapdesc *		mapdesc;	/* map descriptor */
    REAL *		cpts;		/* control points */
    Quiltspec		qspec[MAXDIM];	/* the dimensional data */
    Quiltspec_ptr	eqspec;		/* qspec trailer */
    Quilt		*next;		/* next quilt in linked list */
			
    void		deleteMe( Pool& );
    void		toBezier( Knotvector &, INREAL *, long  );
    void		toBezier( Knotvector &, Knotvector &, INREAL *, long  );
    void		select( REAL *, REAL * );
    int			getDimension( void ) { return eqspec - qspec; }
    void 		download( Backend & );
    void		downloadAll( REAL *, REAL *, Backend & );
    int 		isCulled( void );
    void		getRange( REAL *, REAL *, Flist&, Flist & );
    void		getRange( REAL *, REAL *, int, Flist & );
    void		getRange( REAL *, REAL *, Flist&  );
    void		findRates( Flist& slist, Flist& tlist, REAL[2] );
    void		findSampleRates( Flist& slist, Flist& tlist );
    void		show();
};

typedef Quilt *Quilt_ptr;

#endif /* __gluquilt_h_ */
