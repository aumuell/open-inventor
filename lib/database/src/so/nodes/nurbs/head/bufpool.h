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

#ifndef __glubufpool_h_
#define __glubufpool_h_

#include "../SoAddPrefix.h"

/*
 * bufpool.h - $Revision: 1.1.1.1 $
 */

#include "myassert.h"
#include "mystdlib.h"
#include <assert.h>

#define NBLOCKS	32

class Buffer {
	friend class 	Pool;
	Buffer	*	next;		/* next buffer on free list	*/
};

class Pool {
public:
			Pool( int, int, char * );
			~Pool( void );
    inline void*	new_buffer( void );
    inline void		free_buffer( void * );
    void		clear( void );
    
private:
    void		grow( void );

protected:
    Buffer		*freelist;		/* linked list of free buffers */
    char		*blocklist[NBLOCKS];	/* blocks of malloced memory */
    int			nextblock;		/* next free block index */
    char		*curblock;		/* last malloced block */
    int			buffersize;		/* bytes per buffer */
    int			nextsize;		/* size of next block of memory	*/
    int			nextfree;		/* byte offset past next free buffer */
    int			initsize;
    enum Magic { is_allocated = 0xf3a1, is_free = 0xf1a2 };
    char		*name;			/* name of the pool */
    Magic		magic;			/* marker for valid pool */
};

/*-----------------------------------------------------------------------------
 * Pool::free_buffer - return a buffer to a pool
 *-----------------------------------------------------------------------------
 */

inline void
Pool::free_buffer( void *b )
{
    assert( this && (magic == is_allocated) );

    /* add buffer to singly connected free list */

    ((Buffer *) b)->next = freelist;
    freelist = (Buffer *) b;
}


/*-----------------------------------------------------------------------------
 * Pool::new_buffer - allocate a buffer from a pool
 *-----------------------------------------------------------------------------
 */

inline void * 
Pool::new_buffer( void )
{
    void *buffer;

    assert( this && (magic == is_allocated) );

    /* find free buffer */

    if( freelist ) {
    	buffer = (void *) freelist; 
    	freelist = freelist->next;
    } else {
    	if( ! nextfree )
    	    grow( );
    	nextfree -= buffersize;;
    	buffer = (void *) (curblock + nextfree);
    }
    return buffer;
}
	
class PooledObj {
public:
    inline void *	operator new( size_t, Pool & );
    inline void * 	operator new( size_t, void *);
    inline void * 	operator new( size_t s)
				{ return ::new char[s]; }
    inline void 	operator delete( void * ) { assert( 0 ); }
    inline void		deleteMe( Pool & );
};

inline void *
PooledObj::operator new( size_t, Pool& pool )
{
    return pool.new_buffer();
}

inline void
PooledObj::deleteMe( Pool& pool )
{
    pool.free_buffer( (void *) this );
}

#endif /* __glubufpool_h_ */
