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

#ifndef __glubasiccrveval_h_
#define __glubasiccrveval_h_

#include "../SoAddPrefix.h"

/*
 * basiccurveeval.h - $Revision: 1.1.1.1 $
 */

#include "types.h"
#include "displaymode.h"
#include "cachingeval.h"

class BasicCurveEvaluator : public CachingEvaluator {
public:
    virtual void	domain1f( REAL, REAL );
    virtual void	range1f( long, REAL *, REAL * );

    virtual void	enable( long );
    virtual void	disable( long );
    virtual void	bgnmap1f( long );
    virtual void	map1f( long, REAL, REAL, long, long, REAL * );
    virtual void	mapgrid1f( long, REAL, REAL );
    virtual void	mapmesh1f( long, long, long );
    virtual void	evalcoord1f( long, REAL );
    virtual void	endmap1f( void );

    virtual void	bgnline( void );
    virtual void	endline( void );
};

#endif /* __glubasiccrveval_h_ */
