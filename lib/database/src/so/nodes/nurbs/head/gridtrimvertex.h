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

#ifndef __glugridtrimvertex_h_
#define __glugridtrimvertex_h_

#include "../SoAddPrefix.h"
/*
 * gridtrimvertex.h - $Revision: 1.1.1.1 $
 */

#include "mystdlib.h"
#include "bufpool.h"
#include "trimvertex.h"
#include "gridvertex.h"

class GridTrimVertex : public PooledObj
{
private:
    TrimVertex	dummyt;
    GridVertex	dummyg;
public:
			GridTrimVertex() { g = 0; t = 0; }
    TrimVertex	*t;
    GridVertex	*g;
   
    inline void		set( long, long );
    inline void		set( REAL, REAL );
    inline void		set( TrimVertex * );
    inline void		clear( void ) { t = 0; g = 0; };
    inline int		isGridVert() { return g ? 1 : 0 ; }
    inline int		isTrimVert() { return t ? 1 : 0 ; }
    inline void		output();
};

inline void
GridTrimVertex::set( long x, long y )
{
    g = &dummyg;
    dummyg.gparam[0] = x;
    dummyg.gparam[1] = y;
}

inline void
GridTrimVertex::set( REAL x, REAL y )
{
    g = 0;
    t = &dummyt;
    dummyt.param[0] = x;
    dummyt.param[1] = y;
    dummyt.nuid = 0;
}

inline void
GridTrimVertex::set( TrimVertex *v )
{
    g = 0;
    t = v;
}

typedef GridTrimVertex *GridTrimVertex_p;
#endif /* __glugridtrimvertex_h_ */
