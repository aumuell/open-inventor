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

/*
 *  bufpool.c 
 *
 *  $Revision: 1.1.1.1 $
 */

#include <assert.h>
#include <stdlib.h>
#include "bufpool.h"
#include "monotone.h"
#include "msort.h"
#include <Inventor/nodes/text/triangulate.h>

/* local functions */
static void	grow_pool( register Bufpool * );

#ifdef PRINT_DEBUG

#define DEBUG_STUFF(stuff) stuff

#if 0
void *
print_malloc(size_t size)
{
    void *result = malloc(size);
    printf("malloc'd %d at location %x\n", size, result);
    return result;
}

void
print_free(void *ptr)
{
    printf("freeing at %x\n", ptr);
    free(ptr);
}

#define malloc print_malloc
#define free print_free
#endif

#else
#define DEBUG_STUFF(stuff)
#endif

/*-----------------------------------------------------------------------------
 * new_pool - allocate a new pool of buffers
 *-----------------------------------------------------------------------------
 */
static Bufpool *
new_pool( int buffersize, int initpoolsize, char *name )
{
    DEBUG_STUFF(printf("new_pool\n");)
    
    register Bufpool *p;
    
    p		= (Bufpool *) malloc( sizeof (Bufpool) );

    p->buffersize= (buffersize < sizeof(Buffer)) ? sizeof(Buffer)	
    						 : buffersize;
    p->nextsize	= initpoolsize * p->buffersize;
#ifndef NDEBUG
    p->name	= name;
    p->magic	= is_allocated;
#endif
    p->nextblock= 0;
    p->curblock	= 0;
    p->freelist	= 0;
    p->nextfree	= 0;
    return p;
}

/*-----------------------------------------------------------------------------
 * new_buffer - allocate a buffer from a pool
 *-----------------------------------------------------------------------------
 */

static char *
new_buffer( register Bufpool *p )
{
    char *buffer;

    assert( p && (p->magic == is_allocated) );

    /* find free buffer */

    if( p->freelist ) {
    	buffer = (char *) p->freelist; 
    	p->freelist = p->freelist->next;
    } else {
    	if( ! p->nextfree )
    	    grow_pool( p );
    	p->nextfree -= p->buffersize;;
    	buffer = p->curblock + p->nextfree;
    }
    return buffer;
}

static void
grow_pool( register Bufpool *p )
{
    DEBUG_STUFF(printf("grow_pool\n");)
    assert( p && (p->magic == is_allocated) );
    p->curblock = (char *) malloc( p->nextsize );
    p->blocklist[p->nextblock++] = p->curblock;
    p->nextfree = p->nextsize;
    p->nextsize *= 2;
}

/*-----------------------------------------------------------------------------
 * free_buffer - return a buffer to a pool
 *-----------------------------------------------------------------------------
 */

static void
free_buffer( Bufpool *p, void *b )
{
    assert( p && (p->magic == is_allocated) );

    /* add buffer to singly connected free list */

    ((Buffer *) b)->next = p->freelist;
    p->freelist = (Buffer *) b;
}

/*-----------------------------------------------------------------------------
 * free_pool - free a pool of buffers and the pool itself
 *-----------------------------------------------------------------------------
 */

static void 
free_pool( Bufpool *p )
{
    DEBUG_STUFF(printf("free_pool\n");)
    
    assert( p && (p->magic == is_allocated) );

    while( p->nextblock )
    	free( p->blocklist[--(p->nextblock)] );
#ifndef NDEBUG
    p->magic = is_free;
#endif
    free( p );
}

/*-----------------------------------------------------------------------------
 * clear_pool - free buffers associated with pool but keep pool 
 *-----------------------------------------------------------------------------
 */

static void 
clear_pool( Bufpool *p )
{
    DEBUG_STUFF(printf("clear_pool\n");)
    
    assert( p && (p->magic == is_allocated) );

    while( p->nextblock )
    	free( p->blocklist[--(p->nextblock)] );
    p->curblock	= 0;
    p->freelist	= 0;
    p->nextfree	= 0;
    if( p->nextsize >= 2 * p->buffersize )
        p->nextsize /= 2;
}





/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* class.c */

/* Derrick Burns - 1989 */


/*----------------------------------------------------------------------------
 * classify - classify a vertex 
 *----------------------------------------------------------------------------
 */

static Vertclass
classify( Vert *vert )
{
    float   det;
    float   ps, pt, ns, nt, ms, mt;

    ps = vert->prev->s;
    pt = vert->prev->t;
    ms = vert->s;
    mt = vert->t;
    ns = vert->next->s;
    nt = vert->next->t;

    if ((ps < ms) && (ms < ns)) return OK_TOP;
    if ((ps > ms) && (ms > ns)) return OK_BOTTOM;

    if (ps == ms) {
	if (ms == ns) {
	    if (pt < mt) 
		 return OK_TOP;
	    else if( pt == mt )
		 return BAD_LONE;
	    else
		return OK_BOTTOM;
	}
	if (pt < mt) {
	    if (ns < ms) return OK_LEFT;
	    return OK_TOP;
	}
	if (pt > mt) {
	    if (ns <= ms) return OK_BOTTOM;
	    return OK_RIGHT;
	}
	return BAD_ERROR;
    }
    if (ms == ns) {
	if (nt < mt) {
	    if (ps >= ms) return OK_BOTTOM;
	    return BAD_RIGHT;
	}
	if (nt > mt) {
	    if (ms >= ps) return OK_TOP;
	    return BAD_LEFT;
	}
	return BAD_ERROR;
    }

    /* Calculate determinant of:
     *
     *     | ps pt 1 |
     *     | ms mt 1 |
     *     | ns nt 1 |
     */

    det = ms*(nt-pt)+ns*(pt-mt)+ps*(mt-nt);

    if ((ps < ms) && (ns < ms)) {
	if (det < 0) return BAD_RIGHT;
	if (det > 0) return OK_LEFT;
	if (det == 0) return BAD_ERROR;
    }
    if ((ps > ms) && (ns > ms)) {
	if (det < 0) return BAD_LEFT;
	if (det > 0) return OK_RIGHT;
	if (det == 0) return BAD_ERROR;
    }
    return BAD_ERROR;
}

/*----------------------------------------------------------------------------
 * unclassify_all - unclassify all vertices in a loop 
 *----------------------------------------------------------------------------
 */

static void
unclassify_all( Vert *vert )
{
    Vert *last = vert;
    do {
	vert->vclass = NO_CLASS;
	vert = vert->next;
    } while( vert != last );
}

/*----------------------------------------------------------------------------
 * classify_all - classify all vertices in a loop 
 *----------------------------------------------------------------------------
 */

static int
classify_all( Vert *vert )
{
    int f = 0;
    Vert *last = vert;

    do {
	vert->vclass = classify( vert );
	f |= vert->vclass;
	vert = vert->next;
    } while( vert != last );
    return f;
}

/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* interface.c */

/* Derrick Burns - 1989 */

TriangulatorObj *
newTriangulator( 
    void	(*bgnpoly)( void * ),
    void	(*endpoly)( void * ),
    void	(*bgntmesh)( void * ),
    void	(*swaptmesh)( void * ),
    void	(*vertex)( void *, void * ),
    void	(*endtmesh)( void * ),
    void	(*error)( char *, void * ),
    void	*userData )
{
    DEBUG_STUFF(printf("newTriangulator\n");)
    
    TriangulatorObj *tobj;
    tobj =( TriangulatorObj *) malloc( sizeof( TriangulatorObj ) );
    tobj->init = 0;
    tobj->minit = 0;
    tobj->in_poly = 0;
    tobj->in_loop = 0;
    tobj->nswaps = 0;
    tobj->vpool = 0;
    tobj->raypool = 0;
    tobj->bgnpoly = bgnpoly;
    tobj->endpoly = endpoly;
    tobj->bgntmesh = bgntmesh;
    tobj->swaptmesh = swaptmesh;
    tobj->endtmesh = endtmesh;
    tobj->vertex = vertex;
    tobj->error = error;
    tobj->parray = NULL;
    tobj->userData = userData;
    return tobj;
}

void
freeTriangulator( TriangulatorObj *t )
{
    DEBUG_STUFF(printf("freeTriangulator\n");)
    
    free( t );
}

/*---------------------------------------------------------------------------
 * in_bgnpoly - called before each input polygon  
 *---------------------------------------------------------------------------
 */

void
in_bgnpoly( TriangulatorObj *tobj, int32_t count )
{
    if( setjmp( tobj->in_env ) != 0 ) 
	return;

#ifdef PRINT_DEBUG
    printf( "bgnpoly\n" );
#endif

    if( tobj->in_loop ) {
	in_error( tobj, "missing in_endloop/in_endpoly" );
    } else if( tobj->in_poly++ ) {
	in_error( tobj, "missing in_endpoly" );
    } 
  
    init_priorityq( tobj, count );
    init_verts( tobj );
    init_raylist( tobj );

    tobj->flag = tobj->nloops = 0;
}

/*---------------------------------------------------------------------------
 * in_endpoly - called after each input polygon 
 *---------------------------------------------------------------------------
 */

void
in_endpoly( TriangulatorObj *tobj )
{
    if( setjmp( tobj->in_env ) != 0 ) 
	return;
     
#ifdef PRINT_DEBUG
    printf( "endpoly\n" );
#endif

    if( tobj->in_loop ) {
	in_error( tobj, "missing in_endloop" );
    } else if( --tobj->in_poly ) {
	in_error( tobj, "missing in_bgnpoly" );
    }

    /* process( tobj->nloops, tobj->looptype, tobj->flag, tobj->head ); */

    (*tobj->bgnpoly)( tobj->userData );
    if( tobj->nloops == 1 ) {
	if( tobj->looptype == TRI_UNKNOWN )
	    tobj->flag |= orient_vert( tobj->head, TRI_EXTERIOR );
	if( (tobj->flag & (BAD | ERROR | LONE) ) == 0 ) {
	    checktriangulate( tobj, tobj->head );
	    triangulateloop( tobj, tobj->head );
	} else
	    monotonize( tobj );
    } else {
 	monotonize( tobj );
    }
    (*tobj->endpoly)( tobj->userData );
    free_verts( tobj );
    free_priorityq( tobj );
    free_raylist( tobj );
}

/*---------------------------------------------------------------------------
 * in_bgnloop - called before each input boundary loop
 *---------------------------------------------------------------------------
 */

void
in_bgnloop( TriangulatorObj *tobj, short type )
{
    if( setjmp( tobj->in_env ) != 0 ) 
	return;
     
#ifdef PRINT_DEBUG
    printf( "bgnloop\n" );
#endif

    if( ! tobj->in_poly ) {
	in_error( tobj, "missing in_bgnpoly" );
    } else if( tobj->in_loop++ ) {
	in_error( tobj, "missing in_endloop" );
    }

    tobj->head = 0;
    tobj->looptype = type;
}

/*---------------------------------------------------------------------------
 * in_endloop - called after each input boundary loop
 *---------------------------------------------------------------------------
 */

void
in_endloop( TriangulatorObj *tobj )
{
    if( setjmp( tobj->in_env ) != 0 ) 
	return;
     
#ifdef PRINT_DEBUG
    printf( "endloop\n" );
#endif

    if( ! tobj->in_poly ) {
	in_error( tobj, "missing in_bgnpoly" );
    } else if( --tobj->in_loop ) {
	in_error( tobj, "missing in_bgnloop" );
    }

    tobj->nloops++;
    tobj->flag |= orient_vert( tobj->head, tobj->looptype );
}

/*---------------------------------------------------------------------------
 * in_vertex - called for each input vertex
 *---------------------------------------------------------------------------
 */

void
in_vertex( TriangulatorObj *tobj, float s, float t, void *data )
{
    Vert *vert;

    if( setjmp( tobj->in_env ) != 0 ) 
	return;
     
    vert = new_vert( tobj );

#ifdef PRINT_DEBUG
    printf( "vertex %g %g\n", s, t );
#endif

    vert->s = s;
    vert->t = t;
    vert->ray = 0;
    vert->data = data;
#ifdef ADDED
    vert->added = 0;
#endif
    if( tobj->head == 0 ) {
	tobj->head = vert->prev = vert->next = vert;
    } else {
        vert->prev = tobj->head->prev;
        vert->next = tobj->head;
        vert->prev->next = vert;
        vert->next->prev = vert;
    }
    add_priorityq( tobj, vert );
}

/*----------------------------------------------------------------------------
 * in_error - data input error, free all storage 
 *----------------------------------------------------------------------------
 */

void
in_error( TriangulatorObj *tobj, char *str )
{
    clear_sort( tobj );
    clear_triangulate( tobj );
    free_raylist( tobj );
    free_verts( tobj );
    free_priorityq( tobj );
    tobj->in_loop = tobj->in_poly = 0;
    (*tobj->error)( str, tobj->userData );
    longjmp( tobj->in_env, (int) str );
}

/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* monotonize.c */

/* Derrick Burns - 1989 */

static Vert *	connectedge( TriangulatorObj *, Vert *, Vert * );

/*----------------------------------------------------------------------------
 * monotonize - add edges to a polygon to create monotone pieces
 *----------------------------------------------------------------------------
 */

static void
monotonize(  TriangulatorObj *tobj )
{
    Ray		*ray, *new_ray1, *new_ray2;

    sort_priorityq( tobj );
    while( more_priorityq( tobj ) ) {
	Vert *vert = remove_priorityq( tobj );

 	if( vert->vclass == NO_CLASS ) {
	    ray = findray_raylist( tobj, vert );
	    if( ray->orientation == ccw_vert( vert ) )
		reverse_vert( vert );
	    (void) classify_all( vert );
	}

	switch (vert->vclass) {

	    case OK_RIGHT: /* two new rays */

		assert( vert->ray == 0 );

	        ray = findray_raylist( tobj, vert );

		if( ray->orientation != 0 )
		    in_error( tobj, "bad orientation or intersecting edges" );

		/* compute top ray */
		new_ray1 = new_ray( tobj, 0 );
		recalc_ray(new_ray1, vert, vert->prev);
		if( vert->prev->vclass != BAD_RIGHT )
			vert->prev->ray = new_ray1;

		/* compute middle ray */
		new_ray2 = new_ray( tobj, 1 );
		recalc_ray(new_ray2, vert, vert->next);
		if( vert->next->vclass != OK_LEFT )
			vert->next->ray = new_ray2;

		new_ray2->vertex = vert;

		assert( ! ray->mustconnect );

		add2_raylist( ray->prev, new_ray1, new_ray2 );

		break;

	    case BAD_LEFT: /* two new rays */

		assert( vert->ray == 0 );

	 	ray = findray_raylist( tobj, vert );

		if( ray->orientation != 1 )
		    in_error( tobj, "bad orientation or intersecting edges" );

		/* compute top ray */
		new_ray1 = new_ray( tobj, 1 );
		recalc_ray(new_ray1, vert, vert->next);
		if( vert->next->vclass != OK_LEFT )
			vert->next->ray = new_ray1;

		/* compute middle ray */
		new_ray2 = new_ray( tobj, 0 );
		recalc_ray(new_ray2, vert, vert->prev);
		if( vert->prev->vclass != BAD_RIGHT )
			vert->prev->ray = new_ray2;

		new_ray1->vertex = connectedge( tobj,vert, ray->vertex);

		/* update bottom ray */
		ray->mustconnect = 0;
		ray->vertex = vert;

		add2_raylist( ray->prev, new_ray1, new_ray2 );

		break;

	    case OK_LEFT:   /* two rays disappear */
		ray = vert->ray;
		if( ! ray ) 
		    in_error( tobj, "vertex/edge intersection" );
		checkray_vert( tobj, vert, ray->prev, ray->next->next );
		/* region above_ray top ray is outside */
		assert( ! ray->mustconnect ); 
		assert( ray->next ); 

		/* region above_ray middle ray is inside */
		if( ray->next->mustconnect ) {
		    triangulateloop( tobj,connectedge( tobj,vert,ray->next->vertex));
		    triangulateloop( tobj, vert );
		} else { 
		    triangulateloop( tobj, vert );
		}

		/* region above_ray bottom ray is outside */
		assert( ray->next->next );
		assert( ! ray->next->next->mustconnect );

		remove2_raylist( tobj, ray );
		delete_ray( tobj, ray->next );
		delete_ray( tobj, ray );
		break;

	    case BAD_RIGHT: /* two rays disappear */
		ray = vert->ray;
		if( ! ray ) 
		    in_error( tobj, "vertex/edge intersection" );
		checkray_vert( tobj, vert, ray->prev, ray->next->next );
		if (ray->mustconnect) {
		    vert = connectedge( tobj, vert, ray->vertex );
		    triangulateloop( tobj, ray->vertex->next );
		} 

		assert( ray->next ); 
		assert( ! ray->next->mustconnect ); 
		assert( ray->next->next );

		if (ray->next->next->mustconnect)
		    triangulateloop( tobj, connectedge( tobj, vert,
					ray->next->next->vertex) );
		ray->next->next->vertex = vert;
		ray->next->next->mustconnect = 1;
		remove2_raylist( tobj, ray );
		delete_ray( tobj, ray->next );
		delete_ray( tobj, ray );
		break;

	    case OK_TOP: /* one ray changes ends and coords */
		ray = vert->ray;
		if( ! ray ) 
		    in_error( tobj, "vertex/edge intersection" );
		checkray_vert( tobj, vert, ray->prev, ray->next ); 
		recalc_ray( ray, vert, vert->next );
		if( vert->next->vclass != OK_LEFT )
			vert->next->ray = ray;

		if (ray->mustconnect) {
		    ray->vertex = connectedge( tobj,vert,ray->vertex);
		    ray->mustconnect = 0;
		    triangulateloop( tobj, vert );
		} else {
		    ray->vertex = vert;
		}

		assert( ray->next );
		assert( ! ray->next->mustconnect );
		
		break;

	    case OK_BOTTOM: /* one ray changes ends and coords */
		ray = vert->ray;
		if( ! ray ) 
		    in_error( tobj, "vertex/edge intersection" );
		checkray_vert( tobj, vert, ray->prev, ray->next ); 
		recalc_ray( ray, vert, vert->prev );
		if( vert->prev->vclass != BAD_RIGHT )
			vert->prev->ray = ray;

		assert( ! ray->mustconnect );
		assert( ray->next );

		if (ray->next->mustconnect) {
		    triangulateloop( tobj,connectedge( tobj,vert, ray->next->vertex));
		    ray->next->mustconnect = 0;
		    ray->next->vertex = vert;
		} else {
		    ray->next->vertex = vert;
		}

		break;

	    case BAD_LONE:
		break;

	    case BAD_ERROR:
		in_error( tobj, "vertex/edge intersection" );
		return;

	    case NO_CLASS:
		assert( 0 );
		break;
	}
    }
    return;
}
 
/*----------------------------------------------------------------------------
 * connectedge - create two anti-parallel edges splitting a polygon
 *----------------------------------------------------------------------------
 */

static Vert *
connectedge( TriangulatorObj *tobj, Vert *x, Vert *y )
{
    Vert	*newx, *newy;

    if( x == 0 || y == 0 || y->prev == 0 || x->next == 0 ) {
	in_error( tobj, "misoriented or self-intersecting loops" );
 	return 0;
    } else {
	assert( x->prev->next == x );
	assert( x->next->prev == x );
	assert( y->prev->next == y );
	assert( y->next->prev == y );
	assert( x->next != y );
	assert( x != y );

        newx = (Vert *) new_vert( tobj );
 	newx->ray = x->ray;
 	newx->vclass = x->vclass;
#ifdef ADDED
 	newx->added = x->added;
#endif
 	newx->s = x->s;
 	newx->t = x->t;
 	newx->data = x->data;

        newy = (Vert *) new_vert( tobj );
 	newy->ray = y->ray;
 	newy->vclass = y->vclass;
#ifdef ADDED
        newy->added = 1;
#endif
 	newy->s = y->s;
 	newy->t = y->t;
 	newy->data = y->data;

        newx->prev = newy;
	newx->next = x->next;
        newy->next = newx;
	newy->prev = y->prev;
	newx->next->prev = newx;
	newy->prev->next = newy;
        x->next = y;
        y->prev = x;
#ifdef ADDED
        x->added = 1;
#endif
	assert( x->prev->next == x );
	assert( x->next->prev == x );
	assert( y->prev->next == y );
	assert( y->next->prev == y );
	assert( newx->prev->next == newx );
	assert( newx->next->prev == newx );
	assert( newy->prev->next == newy );
	assert( newy->next->prev == newy );
        return newx;
    }
}


/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* msort.c */

/*
	Tom Davis - 1988
	Derrick Burns - 1989
 */


/* code to do a merge sort where we assume that the initial data tends
 * to make long runs, either up or down.
 */

#define INITSORT 50


/*---------------------------------------------------------------------------
 * init_sort - initialize merge sort data structures
 *---------------------------------------------------------------------------
 */

static void
init_sort( TriangulatorObj *tobj, int32_t n )
{
    DEBUG_STUFF(printf("init_sort\n");)
    
    if( n == 0 ) return;
    if( tobj->minit ) {
	if( n > tobj->size ) {
	    free( tobj->ptrlist );
	    free( tobj->limits );	
	    free( tobj->dirs );
	    tobj->size = n * 2;
            tobj->ptrlist = (void **)
		malloc((unsigned int)tobj->size*sizeof(void *) );
            tobj->limits = (int32_t *)
		malloc((unsigned int)(tobj->size+1)*sizeof(int32_t) );
            tobj->dirs = (enum updown *)
		malloc((unsigned int)tobj->size*sizeof(enum updown) );
	} 
    } else {
        tobj->size = n;
        tobj->ptrlist = (void **)
		malloc((unsigned int)tobj->size*sizeof(void *) );
        tobj->limits = (int32_t *)
		malloc((unsigned int)(tobj->size+1)*sizeof(int32_t) );
        tobj->dirs = (enum updown *)
		malloc((unsigned int)tobj->size*sizeof(enum updown) );
        tobj->minit = 1;
    }
}

/*---------------------------------------------------------------------------
 * clear_sort - free merge sort data structures
 *---------------------------------------------------------------------------
 */

static void 
clear_sort( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("clear_sort\n");)
    
    if( tobj->minit ) {
        free( tobj->ptrlist );
	free( tobj->limits );	
	free( tobj->dirs );
        tobj->minit = 0;
    }
 }


/*---------------------------------------------------------------------------
 * msort - perform a merge sort on the data
 *---------------------------------------------------------------------------
 */

static void
msort( TriangulatorObj *tobj, 
       void **curptrlist, int32_t count, int32_t , SortFunc comp )
{
    int32_t i;
    int32_t p1s, p1e, p2s, p2e, d1, d2;
    int32_t q;
    void *tmp;
    void **temp;
    void **newptrlist, **saveptrlist;
    void **c1s, **c1e, **c2s, **c2e, **np;

    if( count == 1) return;
    if( count == 2) {
	if( comp( &curptrlist[0], &curptrlist[1] ) < 0 ) {
	    tmp = curptrlist[0];
	    curptrlist[0] = curptrlist[1];
	    curptrlist[1] = tmp;
	}
	return;
    }

    init_sort( tobj, count );

    saveptrlist = curptrlist;
    newptrlist = tobj->ptrlist;
    tobj->limitcount = 0;
    tobj->limits[0] = 0;

    i=0;
    while( 1 ) {
	do {
	    i++;
	    if( i == count ) break;
	} while( comp(&curptrlist[i-1], &curptrlist[i]) > 0 );
        tobj->dirs[tobj->limitcount] = down;
        tobj->limits[++tobj->limitcount] = i;
	if( i == count ) break;

	do {
	    i++;
	    if( i == count ) break;
	} while( comp(&curptrlist[i-1], &curptrlist[i]) <= 0 );
        tobj->dirs[tobj->limitcount] = up;
        tobj->limits[++tobj->limitcount] = i;
	if( i == count ) break;
    }

    q = tobj->newlimitcount = 0;
    for (i = 0; i < tobj->limitcount-1; i += 2) {
	if (tobj->dirs[i] == up) {
	    p1s = tobj->limits[i];
	    p1e = tobj->limits[i+1];
	    d1 = 1;
	} else {
	    p1s = tobj->limits[i+1]-1;
	    p1e = tobj->limits[i]-1;
	    d1 = -1;
	}
	if (tobj->dirs[i+1] == up) {
	    p2s = tobj->limits[i+1];
	    p2e = tobj->limits[i+2];
	    d2 = 1;
	} else {
	    p2s = tobj->limits[i+2]-1;
	    p2e = tobj->limits[i+1]-1;
	    d2 = -1;
	}
	while ((p1s != p1e) && (p2s != p2e)) {
	    if (comp(&curptrlist[p1s], &curptrlist[p2s]) > 0) {
		newptrlist[q++] = curptrlist[p2s];
		p2s += d2;
		if (p2s == p2e) do {
		    newptrlist[q++] = curptrlist[p1s];
		    p1s += d1;
		} while (p1s != p1e);
	    } else {
		newptrlist[q++] = curptrlist[p1s];
		p1s += d1;
		if (p1s == p1e) do {
		    newptrlist[q++] = curptrlist[p2s];
		    p2s += d2;
		} while (p2s != p2e);
	    }
	}
	tobj->limits[++tobj->newlimitcount] = q;
    }

    if (tobj->limitcount & 1) {
	if (tobj->dirs[tobj->limitcount-1] == up) {
	    p1s = tobj->limits[tobj->limitcount-1];
	    p1e = tobj->limits[tobj->limitcount];
	    d1 = 1;
	} else {
	    p1s = tobj->limits[tobj->limitcount] - 1;
	    p1e = tobj->limits[tobj->limitcount-1] - 1;
	    d1 = -1;
	}
	do {
	    newptrlist[q++] = curptrlist[p1s];
	    p1s += d1;
	} while (p1s != p1e);
	tobj->limits[++tobj->newlimitcount] = q;
    }

    tobj->limitcount = tobj->newlimitcount;
    temp = curptrlist;
    curptrlist = newptrlist;
    newptrlist = temp;

    while (tobj->limitcount > 1) {
	np = newptrlist;
	q = tobj->newlimitcount = 0;
	for (i = 0; i < tobj->limitcount-1; i += 2) {
	    c1s = curptrlist + tobj->limits[i];
	    c1e = curptrlist + tobj->limits[i+1];
	    c2s = curptrlist + tobj->limits[i+1];
	    c2e = curptrlist + tobj->limits[i+2];
	    while ((c1s != c1e) && (c2s != c2e)) {
		if (comp(c1s, c2s) > 0) {
		    *np++ = *c2s++;
		    if (c2s == c2e) do {
			*np++ = *c1s++;
		    } while (c1s != c1e);
		} else {
		    *np++ = *c1s++;
		    if (c1s == c1e) do {
			*np++ = *c2s++;
		    } while (c2s != c2e);
		}
	    }
	    tobj->limits[++tobj->newlimitcount] = np - newptrlist;
	}
	if( tobj->limitcount & 1 ) {
	    p1s = tobj->limits[tobj->limitcount-1];
	    p1e = tobj->limits[tobj->limitcount];
	    do {
	        *np++ = curptrlist[p1s++];
	    } while (p1s != p1e);
	    tobj->limits[++tobj->newlimitcount] = np - newptrlist;
	}
	tobj->limitcount = tobj->newlimitcount;
	temp = curptrlist;
	curptrlist = newptrlist;
	newptrlist = temp;
    }

    if (curptrlist != saveptrlist)
	for (i = 0; i < count; i++)
	    saveptrlist[i] = curptrlist[i];

    clear_sort( tobj );
}

static int 	comp_priorityq( Vert **x, Vert **y );
static void 	grow_priorityq( TriangulatorObj *tobj );


static void
init_priorityq( TriangulatorObj *tobj, int32_t s )
{
    DEBUG_STUFF(printf("init_priorityq\n");)
    
    assert( ! tobj->parray ) ;
    tobj->phead = 0;
    tobj->ptail = 0;
    tobj->psize = s > 0 ? s : 1;
    tobj->parray = (Vert **)
	malloc( (unsigned int) (sizeof( Vert *) * tobj->psize) );
}

static void
add_priorityq( TriangulatorObj *tobj, Vert *v )
{
    assert( tobj->parray );
    if( tobj->ptail == tobj->psize ) grow_priorityq( tobj );
    tobj->parray[tobj->ptail++] = v;
}

static int
more_priorityq( TriangulatorObj *tobj )
{
    return tobj->phead != tobj->ptail;
}

static void
sort_priorityq( TriangulatorObj *tobj )
{
    assert( tobj->phead <= tobj->ptail );
    msort( tobj, (void **)tobj->parray+tobj->phead,
	   tobj->ptail-tobj->phead, sizeof(Vert *), (SortFunc) comp_priorityq );
}

static Vert *
remove_priorityq( TriangulatorObj *tobj )
{
    return tobj->parray[tobj->phead++];
}

static void
free_priorityq( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("free_priorityq\n");)
    assert( tobj->parray );
    if( tobj->parray ) { free( tobj->parray ); tobj->parray = 0; }
}

static void
grow_priorityq( TriangulatorObj *tobj )
{
    tobj->psize *= 2;
    tobj->parray = (Vert **)
	realloc( tobj->parray, (unsigned int)(sizeof(Vert*)*tobj->psize) ); 
}

static int
comp_priorityq( Vert **vp1, Vert **vp2 )
{
    float diff = (*vp1)->s - (*vp2)->s;
    if (diff < 0.0) return -1;
    if (diff > 0.0) return 1;
    if (diff == 0.0)
        diff = (*vp1)->t - (*vp2)->t;
    if (diff < 0.0) return -1;
    if (diff > 0.0) return 1;

    return 0;
}

/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* ray.c */

/* Derrick Burns - 1989 */

#define ZERO 0.000001

/*----------------------------------------------------------------------------
 * init_raylist - create dummy rays at y = +/- infinity
 *----------------------------------------------------------------------------
 */

static void
init_raylist( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("init_raylist\n");)
    
    Ray *ray1, *ray2;

    if( ! tobj->raypool )
	tobj->raypool = new_pool( sizeof( Ray ), 32, "tobj->raypool" );
    else
        clear_pool( tobj->raypool );

    ray1 = new_ray( tobj, 1 );
    ray2 = new_ray( tobj, 0 );
    ray1->next = ray1->prev = ray2;
    ray2->next = ray2->prev = ray1;
    ray1->coords[0] = ray1->coords[1] = 0.0; ray1->coords[2] = -1.0;
    ray2->coords[0] = ray2->coords[1] = 0.0; ray2->coords[2] = 1.0;
    ray1->vertex =  0;
    ray2->vertex = 0;

#ifdef LAZYRECALC
    ray1->end1 = ray1->end2 = ray2->end1 = ray2->end2 = 0;
#endif

    tobj->raylist = ray1;
}

/*----------------------------------------------------------------------------
 * add2_raylist - add two rays to the ray list
 *----------------------------------------------------------------------------
 */

static void
add2_raylist( Ray *prev, Ray *ray1, Ray *ray2 )
{
    ray1->prev = prev;
    ray1->next = ray2;
    ray2->prev = ray1;
    ray2->next = prev->next;
    ray1->prev->next = ray1;
    ray2->next->prev = ray2;
}

/*----------------------------------------------------------------------------
 * remove2_raylist - remove two rays from the ray list
 *----------------------------------------------------------------------------
 */

static void 
remove2_raylist( TriangulatorObj *tobj, Ray *ray )
{

    if( ray == tobj->raylist ||
	ray == tobj->raylist->prev ||
	ray->next == tobj->raylist ) {
	in_error( tobj, "illegal data" );
    } else {
        ray->prev->next = ray->next->next;
        ray->next->next->prev = ray->prev;
   }
}

/*----------------------------------------------------------------------------
 * findray_raylist - find first ray below given vertex
 *----------------------------------------------------------------------------
 */

static Ray *
findray_raylist( TriangulatorObj *tobj, Vert *v )
{
    Ray *ray;
    for( ray=tobj->raylist; above_ray(ray, v) < -ZERO; ray=ray->next );
    return ray;
}

/*----------------------------------------------------------------------------
 * free_raylist - reclaim all rays
 *----------------------------------------------------------------------------
 */

static void
free_raylist( TriangulatorObj *tobj ) 
{
    DEBUG_STUFF(printf("free_raylist\n");)
    free_pool( tobj->raypool );
}

/*----------------------------------------------------------------------------
 * new_ray - create a new ray
 *----------------------------------------------------------------------------
 */

static Ray *
new_ray( TriangulatorObj *tobj, int orientation )
{
    Ray	*ray = (Ray *) new_buffer( tobj->raypool );
    ray->orientation = orientation;
    ray->mustconnect = 0;
    ray->vertex = 0;
    return ray;
}

/*----------------------------------------------------------------------------
 * delete_ray - free ray
 *----------------------------------------------------------------------------
 */

static void
delete_ray( TriangulatorObj *tobj, Ray *ray )
{
    free_buffer( tobj->raypool, ray );
}

/*----------------------------------------------------------------------------
 * above_ray - determine if a vertex is above_ray a ray
 *----------------------------------------------------------------------------
 */

static int 
above_ray( Ray *ray, Vert *vert )
{
    /* returns 1 if the vertex is above_ray the ray (is in the region
     * associated with the ray) 0 if it's on the ray, and -1 if it is
     * below. */

    float dot;

#ifdef LAZYRECALC
    if( ray->end1 != 0 ) {
        ray->coords[0] = ray->end1->t - ray->end2->t;
        ray->coords[1] = ray->end2->s - ray->end1->s;
	ray->coords[2] = - ray->coords[1] * ray->end2->t 
			 - ray->coords[0] * ray->end2->s;
        ray->end1 = ray->end2 = 0;
    }
#endif 

    dot = ray->coords[0]*vert->s + ray->coords[1]*vert->t + ray->coords[2];
    if (dot > 0.0) return 1;
    if (dot < 0.0) return -1;
    /* XXX
	If we reach this point, it means that an input vertex lies on
	an edge connecting two other veritces.  This is officially
	disallowed. In some cases this will be detected in monotonize
	and in others it won't.  When it is not detected, it will NOT
	cause the program to die, and it WILL give reasonable results.
    */
    return 0;
}

/*----------------------------------------------------------------------------
 * recalc_ray - calculate the vector perpindicular to the line through v0/v1
 *----------------------------------------------------------------------------
 */

static void
recalc_ray( Ray *ray, Vert *v0, Vert *v1 )
{
#ifdef LAZYRECALC
    ray->end1 = v0;
    ray->end2 = v1;
#else
    ray->coords[0] = v0->t - v1->t;
    ray->coords[1] = v1->s - v0->s;
    ray->coords[2] = - ray->coords[1]*v1->t - ray->coords[0]*v1->s;
#endif
}

/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* triangulate.c */

/* Derrick Burns - 1989 */

#define do_out_vert(x,y) do_out_vertex(x, (y)->data )
#define EPSILON 0.0

static void		do_out_bgntmesh( TriangulatorObj * );
static void		do_out_swaptmesh( void );
static void		do_out_vertex( TriangulatorObj *, void * );
static void		do_out_endtmesh( TriangulatorObj * );
static void		checkabove( TriangulatorObj *, Vert *, Vert *, Vert *);

/*----------------------------------------------------------------------------
 * init_triangulate - reinitialize triangulation data structures
 *----------------------------------------------------------------------------
 */

static void
init_triangulate( TriangulatorObj *tobj, int32_t nverts )
{
   DEBUG_STUFF(printf("init_triangulate\n");)
   
   if( tobj->init ) {
	if( nverts > tobj->vcount ) {
	    myfree( (char *)tobj->vdata );
            tobj->vcount = nverts * 2;
            tobj->vdata = (Vert **)
		mymalloc( (unsigned int) tobj->vcount * sizeof(Vert *) );
	} 
   } else {
     tobj->init = 1;
     tobj->vcount = nverts;
     tobj->vdata = (Vert **)
	mymalloc( (unsigned int) tobj->vcount * sizeof(Vert *) );
  }
}

/*----------------------------------------------------------------------------
 * clear_triangulate - free triangulation data structures
 *----------------------------------------------------------------------------
 */

static void
clear_triangulate( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("clear_triangulate\n");)
    
    if( tobj->init == 1 ) {
        myfree( (char *) tobj->vdata );
        tobj->init = 0;
    }
}

/*----------------------------------------------------------------------------
 * nextuppervert - find next vertex on upper chain
 *----------------------------------------------------------------------------
 */

static Vert *
nextuppervert( TriangulatorObj *tobj )
{
    return (tobj->vtop == tobj->vlast || tobj->vbottom == tobj->vtop->prev)
	   ? 0 : tobj->vtop->prev;
}

/*----------------------------------------------------------------------------
 * nextlowervert - find next vertex on lower chain 
 *----------------------------------------------------------------------------
 */

static Vert *
nextlowervert( TriangulatorObj *tobj )
{
    return (tobj->vbottom == tobj->vlast || tobj->vtop == tobj->vbottom->next)
	   ? 0 : tobj->vbottom->next;
}

/*----------------------------------------------------------------------------
 * testccw_vert - test if top three verts on stack make ccw_vert turn
 *----------------------------------------------------------------------------
 */

static int
testccw_vert( TriangulatorObj *tobj ) /* tobj->lastedge == 1 */
{
    float s0, t0, s1, t1, s2, t2;
    float area;

    s0 = tobj->vdata[tobj->vdatalast]->s;
    t0 = tobj->vdata[tobj->vdatalast]->t;
    s1 = tobj->vdata[tobj->vdatatop-1]->s;
    t1 = tobj->vdata[tobj->vdatatop-1]->t;
    s2 = tobj->vdata[tobj->vdatatop-2]->s;
    t2 = tobj->vdata[tobj->vdatatop-2]->t;

    area = s0*(t1-t2) - s1*(t0-t2) + s2*(t0-t1);
    return (area < -EPSILON) ? 0 : 1;
}

/*----------------------------------------------------------------------------
 * testcw - test if top three verts on stack make cw turn
 *----------------------------------------------------------------------------
 */

static int
testcw( TriangulatorObj *tobj ) /* tobj->lastedge == 0 */
{
    float s0, t0, s1, t1, s2, t2;
    float area;

    s0 = tobj->vdata[tobj->vdatalast]->s;
    t0 = tobj->vdata[tobj->vdatalast]->t;
    s1 = tobj->vdata[tobj->vdatatop-1]->s;
    t1 = tobj->vdata[tobj->vdatatop-1]->t;
    s2 = tobj->vdata[tobj->vdatatop-2]->s;
    t2 = tobj->vdata[tobj->vdatatop-2]->t;
    area = s0*(t1-t2) - s1*(t0-t2) + s2*(t0-t1);
    return (area > EPSILON) ? 0 : 1;
}


/*----------------------------------------------------------------------------
 * pushvert - place vertex on stack
 *----------------------------------------------------------------------------
 */

static void 
pushvert( TriangulatorObj *tobj, Vert *v )
{
    ++tobj->vdatatop;
    assert( tobj->vdatatop < tobj->vcount );
    tobj->vdata[tobj->vdatatop] = v;
}


/*----------------------------------------------------------------------------
 * addedge - process new vertex 
 *----------------------------------------------------------------------------
 */

static void
addedge( TriangulatorObj *tobj, Vert *v, int32_t edge )
{

    pushvert( tobj, v );
    if( tobj->vdatatop < 2 ) {
	tobj->lastedge = edge;
	return;
    }

    tobj->vdatalast = tobj->vdatatop;

    if( tobj->lastedge != edge ) {
	if( tobj->lastedge == 1 ) {

	    if( tobj->last[0] == tobj->vdata[1] &&
		tobj->last[1] == tobj->vdata[0] ) {
	        do_out_swaptmesh();
	        do_out_vert( tobj, tobj->vdata[tobj->vdatalast] );
	        for( tobj->vdatatop = 2;
		     tobj->vdatatop < tobj->vdatalast;
		     tobj->vdatatop++ ) {
	            do_out_vert( tobj, tobj->vdata[tobj->vdatatop] );
	            do_out_swaptmesh();
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast-1];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "1a\n" );
#endif
	    } else 
	    if( tobj->last[0] == tobj->vdata[tobj->vdatalast-1] &&
		tobj->last[1]==tobj->vdata[tobj->vdatalast-2]) {
		do_out_vert( tobj, tobj->vdata[tobj->vdatalast] );
	        do_out_swaptmesh();
	        for( tobj->vdatatop = tobj->vdatalast-3; tobj->vdatatop >= 0; tobj->vdatatop-- ) {
	            do_out_swaptmesh();
	            do_out_vert( tobj, tobj->vdata[tobj->vdatatop] );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[0];
#ifdef TRACE
		printf( "1b\n" );
#endif
	    } else
	    {
		if( tobj->last[0] != 0 ) do_out_endtmesh( tobj );
	        do_out_bgntmesh( tobj );
	        do_out_vert( tobj,  tobj->vdata[0] );
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        for( tobj->vdatatop = 1; tobj->vdatatop < tobj->vdatalast; tobj->vdatatop++ ) {
	            do_out_vert( tobj,  tobj->vdata[tobj->vdatatop] );
	            do_out_swaptmesh();
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast-1];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "1c\n" );
#endif
	    }

	    tobj->lastedge = 0;
	} else {
	    if( tobj->last[0]==tobj->vdata[0] &&
		tobj->last[1]==tobj->vdata[1] ) {
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        do_out_swaptmesh();
	        for( tobj->vdatatop = 2; tobj->vdatatop < tobj->vdatalast; tobj->vdatatop++ ) {
	            do_out_swaptmesh();
	            do_out_vert( tobj,  tobj->vdata[tobj->vdatatop] );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast-1];
#ifdef TRACE
		printf( "2a\n" );
#endif
	    } else 
	    if( tobj->last[0]==tobj->vdata[tobj->vdatalast-2] &&
		tobj->last[1]==tobj->vdata[tobj->vdatalast-1] ) {
	        do_out_swaptmesh();
		do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        for( tobj->vdatatop = tobj->vdatalast-3; tobj->vdatatop >= 0; tobj->vdatatop-- ) {
	            do_out_vert( tobj,  tobj->vdata[tobj->vdatatop] );
	            do_out_swaptmesh();
	        }
	        tobj->last[0] = tobj->vdata[0];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "2b\n" );
#endif
	    } else {
		if( tobj->last[0] != 0 ) do_out_endtmesh( tobj );
	    	do_out_bgntmesh( tobj );
	        do_out_vert( tobj, tobj->vdata[tobj->vdatalast]);
		do_out_vert( tobj, tobj->vdata[0]);
	        for( tobj->vdatatop = 1; tobj->vdatatop < tobj->vdatalast; tobj->vdatatop++ ) {
	            do_out_swaptmesh();
	            do_out_vert( tobj,  tobj->vdata[tobj->vdatatop] );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast-1];
#ifdef TRACE
		printf( "2c\n" );
#endif
	    }
	    tobj->lastedge = 1;
	}

	tobj->vdata[0] = tobj->vdata[tobj->vdatalast-1];
	tobj->vdata[1] = tobj->vdata[tobj->vdatalast];
	tobj->vdatatop = 1;
    } else {
	if( tobj->lastedge == 1 ) {
	    int32_t i;
	    if( ! testccw_vert( tobj ) ) return;
	    do {
		tobj->vdatatop--;
	    } while( (tobj->vdatatop > 1) && testccw_vert( tobj ) );

	    if( tobj->last[0]==tobj->vdata[tobj->vdatalast-1] &&
		tobj->last[1]==tobj->vdata[tobj->vdatalast-2] ) {
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        do_out_swaptmesh();
		for( i=tobj->vdatalast-3; i>=tobj->vdatatop-1; i-- ) {
	            do_out_swaptmesh();
	            do_out_vert( tobj,  tobj->vdata[i] );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[tobj->vdatatop-1];
#ifdef TRACE
		printf( "3a\n" );
#endif
	    } else
	    if( tobj->last[0] == tobj->vdata[tobj->vdatatop] &&
		tobj->last[1] == tobj->vdata[tobj->vdatatop-1] ) {
	        do_out_swaptmesh();
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        for( i = tobj->vdatatop+1; i < tobj->vdatalast; i++ ) {
	            do_out_vert( tobj,  tobj->vdata[i] );
	            do_out_swaptmesh();
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast-1];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "3b\n" );
#endif
	    } else {
		if( tobj->last[0] != 0 ) do_out_endtmesh( tobj );
	        do_out_bgntmesh( tobj );
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast-1] );
		for( i=tobj->vdatalast-2; i>=tobj->vdatatop-1; i-- ) {
	            do_out_swaptmesh();
	            do_out_vert( tobj,  tobj->vdata[i] );
	        } 
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[tobj->vdatatop-1];
#ifdef TRACE
		printf( "3c\n" );
#endif
	    }
	} else {
	    int32_t i;
	    if( ! testcw( tobj ) ) return;
	    do {
		tobj->vdatatop--;
	    } while( (tobj->vdatatop > 1) && testcw( tobj ) );

	    if( tobj->last[0] == tobj->vdata[tobj->vdatalast-2] &&
		tobj->last[1]==tobj->vdata[tobj->vdatalast-1] ) {
		do_out_swaptmesh();
		do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        for( i=tobj->vdatalast-3; i>=tobj->vdatatop-1; i--) {
	            do_out_vert( tobj,  tobj->vdata[i] );
	            do_out_swaptmesh( );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatatop-1];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "4a\n" );
#endif
	    } else
	    if( tobj->last[0] == tobj->vdata[tobj->vdatatop-1] &&
		tobj->last[1] == tobj->vdata[tobj->vdatatop] ) {
		do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
		do_out_swaptmesh();
	        for( i=tobj->vdatatop+1; i<tobj->vdatalast; i++ ) {
	            do_out_swaptmesh( );
	            do_out_vert( tobj,  tobj->vdata[i] );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatalast];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast-1];
#ifdef TRACE
		printf( "4b\n" );
#endif
	    } else {
	        if( tobj->last[0] != 0 ) do_out_endtmesh( tobj );
	        do_out_bgntmesh( tobj );
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast-1] );
	        do_out_vert( tobj,  tobj->vdata[tobj->vdatalast] );
	        for( i=tobj->vdatalast-2; i>=tobj->vdatatop-1; i-- ) {
	            do_out_vert( tobj,  tobj->vdata[i] );
	            do_out_swaptmesh( );
	        }
	        tobj->last[0] = tobj->vdata[tobj->vdatatop-1];
	        tobj->last[1] = tobj->vdata[tobj->vdatalast];
#ifdef TRACE
		printf( "4c\n" );
#endif
	    }
	}
	tobj->vdata[tobj->vdatatop] = tobj->vdata[tobj->vdatalast];
    }
}

/*----------------------------------------------------------------------------
 * triangulate - triangulate a monotone loop of vertices 
 *----------------------------------------------------------------------------
 */

void
triangulate( TriangulatorObj *tobj, Vert *v, int32_t count )
{
    Vert	*vnext;

    init_triangulate( tobj, count );
    tobj->vlast = last_vert( v );
    tobj->vdatatop = -1;
    pushvert( tobj, tobj->vbottom = tobj->vtop = first_vert( v ) );
    tobj->vtop 	= nextuppervert( tobj );
    tobj->vbottom = nextlowervert( tobj );

    assert( tobj->vtop && tobj->vbottom );
    tobj->last[0] = tobj->last[1] = 0;

    while ( 1 ) {
	if (tobj->vtop->s < tobj->vbottom->s) {
	    addedge( tobj, tobj->vtop, 1);
	    tobj->vtop = nextuppervert( tobj );
	    if (tobj->vtop == 0) {
		while (tobj->vbottom) {
		    vnext = nextlowervert( tobj );
		    addedge( tobj,  tobj->vbottom, (vnext ? 0 : 2 ) );
		    tobj->vbottom = vnext;
		}
		break;
	    }
	} else {
	    addedge( tobj, tobj->vbottom, 0);
	    tobj->vbottom = nextlowervert( tobj );
	    if (tobj->vbottom == 0) {
		while (tobj->vtop) {
		    vnext = nextuppervert( tobj );
		    addedge( tobj,  tobj->vtop, (vnext ? 1 : 2 ) );
		    tobj->vtop = vnext;
		}
		break;
	    }
	}
    }
    if( tobj->last[0] != 0 )
	do_out_endtmesh( tobj );
    assert( tobj->vdatatop < 2 );
    clear_triangulate( tobj );
}


/*----------------------------------------------------------------------------
 * triangulateloop  - count vertices in loop and split into triangle meshes
 *----------------------------------------------------------------------------
 */

static void
triangulateloop( TriangulatorObj *tobj, Vert *v )
{
    short count = 0;
    Vert  *vl = v;
    do {
	count++;
	v = v->next;
    } while( v != vl );
    triangulate( tobj, v, count );
}



static int nswaps = 0;

static void
do_out_bgntmesh( TriangulatorObj *tobj )
{
    nswaps = 0;
    (*tobj->bgntmesh)( tobj->userData );
}

static void
do_out_swaptmesh( void )
{
    nswaps = 1 - nswaps;
}

static void
do_out_vertex( TriangulatorObj *tobj, void *data )
{
    if( nswaps )
	(*tobj->swaptmesh)( tobj->userData );
    nswaps = 0;
    (*tobj->vertex)( data, tobj->userData );
}

static void
do_out_endtmesh( TriangulatorObj *tobj )
{
    (*tobj->endtmesh)( tobj->userData );
}

static void
checktriangulate( TriangulatorObj *tobj, Vert *v )
{
    Vert	*vb, *vt, *vl;

    vl = last_vert( v );
    vb = vt = first_vert( v );
    vb = vb->next;
    vt = vt->prev;

    if( vb == vt ) return;

    while( 1 ) {
	if( vt->s < vb->s ) {
	    checkabove( tobj, vt, vb->prev, vb );
	    vt = vt->prev;
	    if( vt == vl ) {
		for( ; vb != vl; vb=vb->next ) 
		    checkabove( tobj, vb, vl, vl->next );
		break;
	    }
	} else {
	    checkabove( tobj, vb, vt, vt->next );
	    vb = vb->next;
	    if( vb== tobj->vlast ) {
		for( ; vt != vl; vt=vt->prev )
		    checkabove( tobj, vt, vl->prev, vl );
		break;
	    }
	}
    }
}

static void
checkabove( TriangulatorObj *tobj, Vert *v0, Vert *v1, Vert *v2 )
{
    float area;
    area = v0->s*(v1->t-v2->t) - v1->s*(v0->t-v2->t) + v2->s*(v0->t-v1->t);
    if( area < 0 )
	in_error( tobj, "intersecting edges" );
}
/**************************************************************************
 *									  *
 * 		 Copyright (C) 1989, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* vlist2.c */

/* Derrick Burns - 1989 */

 /*---------------------------------------------------------------------------
 * free_verts - free all vertices and vertex lists 
 *---------------------------------------------------------------------------
 */

static void
free_verts( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("free_verts\n");)
    free_pool( tobj->vpool );
}

/*---------------------------------------------------------------------------
 * init_verts - set initial size of vertex pointer list and vertex lists
 *---------------------------------------------------------------------------
 */

static void
init_verts( TriangulatorObj *tobj )
{
    DEBUG_STUFF(printf("init_verts\n");)
    if( ! tobj->vpool )
	tobj->vpool = new_pool( sizeof( Vert ), INITVPSIZE, "vert_pool" );
}

/*---------------------------------------------------------------------------
 * new_vert - allocate a vertex
 *---------------------------------------------------------------------------
 */

static Vert *
new_vert( TriangulatorObj *tobj )
{
    return ( Vert *) new_buffer( tobj->vpool );
}

/*----------------------------------------------------------------------------
 * checkray_vert - cheap check for ray/ray intersection
 *----------------------------------------------------------------------------
 */

static void
checkray_vert( TriangulatorObj *tobj, Vert *v, Ray *ray1, Ray *ray2 )
{
    if( above_ray( ray2, v ) < 0 || above_ray( ray1, v ) > 0 )
	in_error( tobj, "intersecting edges" );
}

/*---------------------------------------------------------------------------
 * first_vert - find leftmost vertex in loop
 *---------------------------------------------------------------------------
 */

static Vert *
first_vert( Vert *h )
{
    Vert *vlo, *v;
    for( vlo = h, v = h->next; v != h; v=v->next )
	if( (v->s < vlo->s) || ( v->s == vlo->s && v->t < vlo->t ) )
	    vlo = v;
    return vlo;
}

/*----------------------------------------------------------------------------
 * last_vert - find rightmost vertex on loop
 *----------------------------------------------------------------------------
 */

static Vert *
last_vert( Vert *h )
{
    Vert *vhi, *v;
    for( vhi = h, v = h->next; v != h; v=v->next )
	if( (v->s > vhi->s) || ( v->s == vhi->s && v->t > vhi->t ) )
	    vhi = v;
    return vhi;
}

/*---------------------------------------------------------------------------
 * reverse_vert - reverse_vert a doubly linked list
 *---------------------------------------------------------------------------
 */

static void
reverse_vert( Vert *h )
{
    Vert *v, *next;
    v = h;
    do {
	next = v->next;	
	v->next = v->prev;
	v->prev = next;
 	v = next;
    } while( v != h );
}

/*---------------------------------------------------------------------------
 * ccw_vert - check if three vertices are oriented ccw_vert
 *---------------------------------------------------------------------------
 */

static short
ccw_vert( Vert *v )
{
    double det;
    det = (v->s * (v->next->t - v->prev->t)+
	   v->next->s * (v->prev->t - v->t)+
	   v->prev->s * (v->t - v->next->t));
    if( det == 0.0 ) 
	return v->prev->t > v->next->t ? 1 : 0;
    else if( det > 0.0 )
	return 1;
    else 
	return 0;

/*
    if( ABS(det) <= ZERO )
        in_error( "lone point" );
*/
}


/*----------------------------------------------------------------------------
 * orient_vert - correct the orientation of a loop 
 *----------------------------------------------------------------------------
 */

static int32_t
orient_vert( Vert *v, short type )
{
    int32_t f;
    switch( type ) {
	case TRI_EXTERIOR:
 	    if( ! ccw_vert( first_vert( v ) ) ) reverse_vert( v );
	    f = classify_all( v );
	    break;
	case TRI_INTERIOR:
 	    if( ccw_vert( first_vert( v ) ) ) reverse_vert( v  );
	    f = classify_all( v );
	    break;
	case TRI_RIGHT:
	    f = classify_all( v );
	    break;
	case TRI_LEFT:
	    reverse_vert( v );
	    f = classify_all( v );
	    break;
	case TRI_UNKNOWN:
	    unclassify_all( v );
	    f = 0;
	    break;
    }
    return f;
}

