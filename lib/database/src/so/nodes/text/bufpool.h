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

/* bufpool.h */

#ifndef BUFPOOL

#define BUFPOOL
#define	NBLOCKS	32

typedef enum { is_allocated = 0xf3a1, is_free = 0xf1a2 } Magic;

typedef struct _buffer {
	struct	_buffer	*next;		/* next buffer on free list	*/
} Buffer ;

typedef struct Bufpool {
	Buffer	*freelist;	/* linked list of free buffers		*/
	char	*blocklist[NBLOCKS];	/* blocks of malloced memory	*/
	int	nextblock;	/* next free block index		*/
	char	*curblock;	/* last malloced block			*/
	int	buffersize;	/* bytes per buffer			*/
	int	nextsize;	/* size of next block of memory		*/
	int	nextfree;	/* byte offset past next free buffer	*/
#ifndef NDEBUG
	char	*name;		/* name of the pool			*/
	Magic	magic;		/* marker for valid pool		*/
#endif
} Bufpool;

	
static	Bufpool	*new_pool( int, int, char * );
static	char	*new_buffer( Bufpool * );
static	void	free_buffer( Bufpool *, void * );
static	void	clear_pool( Bufpool * );
#endif
