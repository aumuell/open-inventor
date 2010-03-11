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

#ifndef __gludisplaylist_h_
#define __gludisplaylist_h_

#include "../SoAddPrefix.h"

/*
 * displaylist.h - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mysetjmp.h"
#include "mystdio.h"
#include "bufpool.h"

class NurbsTessellator;

typedef void (NurbsTessellator::*PFVS)( void * );

struct Dlnode : public PooledObj {
    			Dlnode( PFVS, void *, PFVS );
    PFVS		work;
    void *		arg;
    PFVS		cleanup;
    Dlnode *		next;
};

inline
Dlnode::Dlnode( PFVS _work, void *_arg, PFVS _cleanup ) 
{
    work = _work;
    arg = _arg;
    cleanup = _cleanup;
}

class DisplayList {
public:
			DisplayList( NurbsTessellator * );
			~DisplayList( void );
    void		play( void );
    void		append( PFVS work, void *arg, PFVS cleanup );
    void		endList( void );
private:
    Dlnode 		*nodes;
    Pool		dlnodePool;
    Dlnode		**lastNode;
    NurbsTessellator 	*nt;
};

#endif /* __gludisplaylist_h_ */
