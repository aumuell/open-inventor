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

#ifndef MONOTONE_H
#define MONOTONE_H
/* monotone.h */

/* Derrick Burns - 1989 */

#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

#define INITVERTS	256
#define INITVPSIZE	256

#define BAD		0x01
#define OK		0x02
#define LEFT		0x04
#define RIGHT		0x08
#define TOP		0x10
#define BOTTOM		0x20
#define LONE		0x40
#define ERROR		0x80

enum _Vertclass {
    NO_CLASS	= 0,
    OK_RIGHT	= OK | RIGHT,
    OK_LEFT	= OK | LEFT, 
    OK_TOP	= OK | TOP, 	
    OK_BOTTOM	= OK | BOTTOM,	 
    BAD_RIGHT	= BAD | RIGHT,
    BAD_LEFT	= BAD | LEFT, 
    BAD_LONE	= BAD | LONE, 
    BAD_ERROR	= BAD | ERROR
};

typedef enum _Vertclass Vertclass;

typedef struct Vert {
    struct Vert	*next;
    struct Vert *prev;
    struct Ray	*ray;
    Vertclass	vclass;
#ifdef ADDED
    char	added;
#endif
    float	s;
    float	t;
    float	v[3];
    void	*data;
} Vert;

typedef struct Ray {
    struct Ray	*next;
    struct Ray	*prev;
    struct Vert	*vertex;
#ifdef LAZYRECALC
    struct Vert	*end1;		/* ray's near endpoint */
    struct Vert	*end2;		/* ray's far endpoint */
#endif
    int		orientation;
    int		mustconnect;	/* 1 if mustconnect, 0 otherwise */
    float	coords[3];	/* ax + by + c.  Assume b >= 0. */
} Ray;

// forward reference
struct TriangulatorObj;

static	void	init_verts( TriangulatorObj * );
static	void	free_verts( TriangulatorObj * );

static	Vert	*new_vert( TriangulatorObj * );
static  Vert	*first_vert( Vert * );
static  Vert	*last_vert( Vert * );
static  short	ccw_vert( Vert * );
static	void	reverse_vert( Vert * );
static	void	checkray_vert( TriangulatorObj *, Vert *, Ray *, Ray * );
static	int32_t	orient_vert( Vert *, short );

static 	Ray *	new_ray( TriangulatorObj *, int );
static	int 	above_ray( Ray *, Vert * );
static	void	recalc_ray( Ray *, Vert *, Vert * );

static  void	init_raylist( TriangulatorObj * );
static 	void	add2_raylist( Ray *, Ray *, Ray * );
static	void	remove2_raylist( TriangulatorObj *, Ray * );
static	void	delete_ray( TriangulatorObj *, Ray * );

static	Ray	*findray_raylist( TriangulatorObj *, Vert *v);
static	void	free_raylist( TriangulatorObj * );

static	void	clear_triangulate( TriangulatorObj * );
extern	void	triangulate( TriangulatorObj *, Vert *, int32_t );
static	void	checktriangulate( TriangulatorObj *, Vert * );

static	void	init_priorityq( TriangulatorObj *, int32_t );
static	void	add_priorityq( TriangulatorObj *,Vert *v );
static	int	more_priorityq( TriangulatorObj * );
static	void	sort_priorityq( TriangulatorObj * );
static	Vert *	remove_priorityq( TriangulatorObj * );
static	void	free_priorityq( TriangulatorObj * );

static	void	unclassify_all( Vert * );
static 	int	classify_all( Vert * );

static void 	monotonize( TriangulatorObj * );
static void 	clear_sort( TriangulatorObj * );
static void	triangulateloop( TriangulatorObj *, Vert *);


#define mymalloc malloc
#define myfree free
#define myrealloc realloc

#endif
