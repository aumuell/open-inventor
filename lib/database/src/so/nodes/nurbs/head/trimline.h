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

#ifndef __glutrimline_h_
#define __glutrimline_h_

#include "../SoAddPrefix.h"
/*
 * trimline.h - $Revision: 1.1.1.1 $
 */

class Arc;
class Backend;

#include "trimvertex.h"
#include "jarcloc.h"


class Trimline {
private:
    TrimVertex**	pts; 	
    long 		numverts;
    long		i;
    long		size;
    Jarcloc		jarcl;
    TrimVertex		t, b;
    TrimVertex 		*tinterp, *binterp;
    void		reset( void ) { numverts = 0; }
    inline void		grow( long );
    inline void		swap( void );
    inline void		append( TrimVertex * );
    static long		interpvert( TrimVertex *, TrimVertex *, TrimVertex *, REAL );



public:
			Trimline();
			~Trimline();
    void		init( TrimVertex * );
    void		init( long, Arc *, long );
    void		getNextPt( void );
    void		getPrevPt( void );
    void		getNextPts( REAL, Backend & );
    void		getPrevPts( REAL, Backend & );
    void		getNextPts( Arc * );
    void		getPrevPts( Arc * );
    inline TrimVertex *	next( void );
    inline TrimVertex *	prev( void ); 
    inline TrimVertex *	first( void );
    inline TrimVertex *	last( void );
};

inline TrimVertex *
Trimline::next( void ) 
{
    if( i < numverts) return pts[i++]; else return 0; 
} 

inline TrimVertex *
Trimline::prev( void ) 
{
    if( i >= 0 ) return pts[i--]; else return 0; 
} 

inline TrimVertex *
Trimline::first( void ) 
{
    i = 0; return pts[i]; 
}

inline TrimVertex *
Trimline::last( void ) 
{
    i = numverts; return pts[--i]; 
}  
#endif /* __glutrimline_h_ */
