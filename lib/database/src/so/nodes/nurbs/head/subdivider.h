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

#ifndef __glusubdivider_h_
#define __glusubdivider_h_

#include "../SoAddPrefix.h"
/*
 * subdivider.h - $Revision: 1.1 $
 */

#include "mysetjmp.h"
#include "bin.h"
#include "flist.h"
#include "slicer.h"
#include "arctess.h"
#include "trimvertex.h"
#include "trimvertpool.h"

class Arc;
class Pool;
class Renderhints;
class Backend;
class Quilt;
class Patchlist;
class Curvelist;
class JumpBuffer;

class Subdivider {
public:
			Subdivider( Renderhints&, Backend& );
			~Subdivider( void );
    void		clear( void );

    void		beginTrims( void ) {}
    void		beginLoop( void );
    void		addArc( REAL *, Quilt *, long );
    void		addArc( int, TrimVertex *, long );
    void		endLoop( void ) {}
    void		endTrims( void ) {}

    void		beginQuilts( void );
    void		addQuilt( Quilt * );
    void		endQuilts( void ) {}

    void		drawCurves( void );
    void		drawSurfaces( long );

    int			ccwTurn_sl( Arc *, Arc * );
    int			ccwTurn_sr( Arc *, Arc * );
    int			ccwTurn_tl( Arc *, Arc * );
    int			ccwTurn_tr( Arc *, Arc * );

    void		setJumpbuffer( JumpBuffer * );

private:
    void		classify_headonleft_s( Bin &, Bin &, Bin &, REAL );
    void		classify_tailonleft_s( Bin &, Bin &, Bin &, REAL );
    void		classify_headonright_s( Bin &, Bin &, Bin &, REAL );
    void		classify_tailonright_s( Bin &, Bin &, Bin &, REAL );
    void		classify_headonleft_t( Bin &, Bin &, Bin &, REAL );
    void		classify_tailonleft_t( Bin &, Bin &, Bin &, REAL );
    void		classify_headonright_t( Bin &, Bin &, Bin &, REAL );
    void		classify_tailonright_t( Bin &, Bin &, Bin &, REAL );

    enum dir 		{ down, same, up, none };
    void		tessellate( Arc *, REAL );
    void		monotonize( Arc *, Bin & );
    int			isMonotone( Arc * );
    int			decompose( Bin &, REAL );


    Slicer		slicer;
    ArcTessellator	arctessellator;
    Pool		arcpool;
    Pool		bezierarcpool;
    Pool		pwlarcpool;
    TrimVertexPool	trimvertexpool;

    JumpBuffer*		jumpbuffer;
    Renderhints&	renderhints;
    Backend&		backend;

    Bin			initialbin;
    Arc *		pjarc;
    int 		s_index;
    int			t_index;
    Quilt *		qlist;
    Flist		spbrkpts;
    Flist		tpbrkpts;
    Flist		smbrkpts;
    Flist		tmbrkpts;
    REAL	 	stepsizes[4];
    int			showDegenerate;
    int			isArcTypeBezier;

    void		samplingSplit( Curvelist&, int );

    void		subdivideInS( Bin&  );
    void		splitInS( Bin&, int, int );
    void		splitInT( Bin&, int, int );
    void		samplingSplit( Bin&, Patchlist&, int, int );
    void		nonSamplingSplit( Bin&, Patchlist&, int, int );
    void		tessellation( Bin&, Patchlist& );
    void		monosplitInS( Bin&, int, int );
    void		monosplitInT( Bin&, int, int );

    void		outline( Bin & );
    void		freejarcs( Bin & );
    void		render( Bin & );
    void		split( Bin &, Bin &, Bin &, int, REAL );
    void		tessellate( Bin &, REAL, REAL, REAL, REAL );

    inline void		setDegenerate( void ) { showDegenerate = 1; }
    inline void		setNonDegenerate( void ) { showDegenerate = 0; }
    inline int		showingDegenerate( void ) { return showDegenerate; }
    inline void		setArcTypeBezier( void ) { isArcTypeBezier = 1; }
    inline void		setArcTypePwl( void ) { isArcTypeBezier = 0; }
    inline int		isBezierArcType( void ) { return isArcTypeBezier; }

    void		makeBorderTrim( const REAL *, const REAL * );
    void		split( Bin &, int, const REAL *, int, int );
    void		partition( Bin &, Bin &, Bin &, Bin &, Bin &, int, REAL );
    void		findIrregularS( Bin & );
    void		findIrregularT( Bin & );


    inline int		bbox( TrimVertex *, TrimVertex *, TrimVertex *, int );
    static int		bbox( REAL, REAL, REAL, REAL, REAL, REAL );
    static int		ccw( TrimVertex *, TrimVertex *, TrimVertex * );
    void		join_s( Bin &, Bin &, Arc *, Arc * );
    void		join_t( Bin &, Bin &, Arc *, Arc * );
    int			arc_split( Arc *, int, REAL, int );
    void		check_s( Arc *, Arc * );
    void		check_t( Arc *, Arc * );
    inline void		link( Arc *, Arc *, Arc *, Arc * );
    inline void		simple_link( Arc *, Arc * );
};

inline void
Subdivider::beginLoop( void ) 
{
    pjarc = 0;
}


#endif /* __glusubdivider_h_ */
