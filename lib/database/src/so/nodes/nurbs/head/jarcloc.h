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

#ifndef __glujarcloc_h_
#define __glujarcloc_h_

#include "../SoAddPrefix.h"
/*
 * jarcloc.h - $Revision: 1.1.1.1 $
 */

#include "arc.h"

class Jarcloc {
private:
    Arc * 		arc;
    TrimVertex		*p;
    TrimVertex		*plast;
public:
    inline void		init( Arc_ptr a, long first, long last ) { arc = a; p=&a->pwlArc->pts[first]; plast = &a->pwlArc->pts[last]; }
    inline TrimVertex *	getnextpt( void );
    inline TrimVertex *	getprevpt( void );
    inline void		reverse();
};

inline void
Jarcloc::reverse()
{
    if( plast == &arc->pwlArc->pts[0] )
	plast =  &arc->pwlArc->pts[arc->pwlArc->npts - 1];
    else
	plast =  &arc->pwlArc->pts[0];
}

inline TrimVertex *
Jarcloc::getnextpt()
{
    assert( p <= plast );
    if( p == plast ) {
	arc = arc->next;
	p = &arc->pwlArc->pts[0];
	plast = &arc->pwlArc->pts[arc->pwlArc->npts - 1];
	assert( p < plast );
    }
    return p++;
}
	
inline TrimVertex *
Jarcloc::getprevpt()
{
    assert( p >= plast );
    if( p == plast ) {
	arc = arc->prev;
	p = &arc->pwlArc->pts[arc->pwlArc->npts - 1];
	plast = &arc->pwlArc->pts[0];
	assert( p > plast );
    }
    return p--;
}
#endif /* __glujarcloc_h_ */
