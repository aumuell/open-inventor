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

#ifndef __gluarc_h_
#define __gluarc_h_

#include "../SoAddPrefix.h"
/*
 * arc.h - $Revision: 1.1 $
 */

#include "myassert.h"
#include "bufpool.h"
#include "mystdio.h"
#include "types.h"
#include "pwlarc.h"
#include "trimvertex.h"

class Bin;
class Arc;
class BezierArc;	
typedef Arc *Arc_ptr;

enum arc_side { arc_none = 0, arc_right, arc_top, arc_left, arc_bottom };


struct Arc : public PooledObj { /* an arc, in two list, the trim list and bin */

    static const int bezier_tag;
    static const int arc_tag;
    static const int tail_tag;

    Arc_ptr		prev;		/* trim list pointer */
    Arc_ptr		next;		/* trim list pointer */
    Arc_ptr		link;		/* bin pointers */
    BezierArc *		bezierArc;	/* associated bezier arc */
    PwlArc *		pwlArc;	/* associated pwl arc */
    long		type;		/* curve type */
    long		nuid;

    inline		Arc( Arc *, PwlArc * );
    inline		Arc( arc_side, long );

    Arc_ptr		append( Arc_ptr );
    int			check( void );
    int			isMonotone( void );
    int			isDisconnected( void );
    int			numpts( void );
    void		markverts( void );
    void		getextrema( Arc_ptr[4] );
    void		print( void );
    void		show( void );
    void		makeSide( PwlArc *, arc_side );

    inline int		isTessellated() { return pwlArc ? 1 : 0; }
    inline long 	isbezier() 	{ return type & bezier_tag; }
    inline void 	setbezier() 	{ type |= bezier_tag; }
    inline void 	clearbezier() 	{ type &= ~bezier_tag; }
    inline long		npts() 		{ return pwlArc->npts; }
    inline TrimVertex *	pts() 		{ return pwlArc->pts; }
    inline REAL * 	tail() 		{ return pwlArc->pts[0].param; }
    inline REAL * 	head() 		{ return next->pwlArc->pts[0].param; }
    inline REAL *	rhead() 	{ return pwlArc->pts[pwlArc->npts-1].param; }
    inline long		ismarked()	{ return type & arc_tag; }
    inline void		setmark()	{ type |= arc_tag; }
    inline void		clearmark()	{ type &= (~arc_tag); }
    inline void		clearside() 	{ type &= ~(0x7 << 8); }
    inline void		setside( arc_side s ) { clearside(); type |= (((long)s)<<8); }
    inline arc_side	getside() 	{ return (arc_side) ((type>>8) & 0x7); }
    inline int		getitail()	{ return type & tail_tag; }
    inline void		setitail()	{ type |= tail_tag; }
    inline void		clearitail()	{ type &= (~tail_tag); }
};

/*--------------------------------------------------------------------------
 * Arc - initialize a new Arc with the same type and uid of
 *	    a given Arc and a given pwl arc
 *--------------------------------------------------------------------------
 */

inline
Arc::Arc( Arc *j, PwlArc *p )
{
    bezierArc = NULL;
    pwlArc = p;
    type = j->type;
    nuid = j->nuid;
}

/*--------------------------------------------------------------------------
 * Arc - initialize a new Arc with the same type and uid of
 *	    a given Arc and a given pwl arc
 *--------------------------------------------------------------------------
 */

inline
Arc::Arc( arc_side side, long _nuid )
{
    bezierArc = NULL;
    pwlArc = NULL;
    type = 0;
    setside( side );
    nuid = _nuid;
}

#endif /* __gluarc_h_ */
