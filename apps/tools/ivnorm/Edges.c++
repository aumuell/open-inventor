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

//
// Code for the edges class, that supports fast querying of the faces
// around an edge.
//

#include <assert.h>
#include "Faces.h"
#include "Edges.h"

EdgeDict::EdgeDict(int initial_size)
{
    assert(initial_size > 0);

    hash_table = new Edge[initial_size];
    for (int i = 0; i < initial_size; i++)
    {
	hash_table[i].v1 = hash_table[i].v2 = (-1);
	hash_table[i].faces.truncate(0);
    }
    n_hash = initial_size;
    n_edges = 0;
}

EdgeDict::~EdgeDict()
{
    delete[] hash_table;
}

void
EdgeDict::Add(Face *f, int32_t v1, int32_t v2)
{
    if (v1 > v2) { int32_t t = v1; v1 = v2; v2 = t; }

    if (v1 == v2)
    {
	fprintf(stderr, "Warning: Degenerate edge found"
		" (v1 = v2 = %d)\n", v1, v2);
	return;
    }
    assert(f != NULL);

    Edge *e = Enter(v1, v2);
    assert(e != NULL);

    e->faces.append(f);
}

void
EdgeDict::OtherFaces(Face *f, int32_t v1, int32_t v2, FaceList &result)
{
    assert(f != NULL);

    result.truncate(0);	// Empty list

    if (v1 == v2) return;	// Degenerate edge, no neighbors

    Edge *e = Find(v1, v2);

    for (int i = 0; i < e->faces.getLength(); i++)
    {
	if (e->faces[i] != f) result.append(e->faces[i]);
    }
}

Edge *
EdgeDict::Enter(int32_t v1, int32_t v2)
{
    assert(v1 != v2);

    int32_t t;
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }

    Edge *e = Find(v1, v2);

    if (e != NULL) return e;

    // Create a new entry.
    // First, see if we need to dynamically increase the hash table's
    // size.
    ++n_edges;
    if (n_edges*2 >= n_hash) ReHash(n_hash*2);

    //
    // Look for an empty spot in the table
    //
    int hpos = HashFunc(v1, v2);
    while (hash_table[hpos].v1 != (-1))
    {
	hpos = (hpos+1) % n_hash;
    }

    hash_table[hpos].v1 = v1;
    hash_table[hpos].v2 = v2;
 
    return hash_table+hpos;
}

//
// These fairly large prime numbers were picked randomly.
//
int
EdgeDict::HashFunc(int32_t v1, int32_t v2)
{
    int result = (v1*1400627 + v2*79823) % n_hash;
    return (result > 0 ? result : -result);
}

void
EdgeDict::ReHash(int newsize)
{
    int i;
    int oldsize = n_hash;
    Edge *old_table = hash_table;

    hash_table = new Edge[newsize];
    for (i = 0; i < newsize; i++)
    {
	hash_table[i].v1 = hash_table[i].v2 = (-1);
	hash_table[i].faces.truncate(0);
    }
    n_hash = newsize;

    for (i = 0; i < oldsize; i++)
    {
	const Edge *old = old_table+i;
	if (old->v1 != (-1))
	{
	    Edge *n = Enter(old->v1, old->v2);
	    n->faces.copy(old->faces);
	}
    }
    delete[] old_table;
}


Edge *
EdgeDict::Find(int32_t v1, int32_t v2)
{
    int32_t t;
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }

    int hpos = HashFunc(v1, v2);

    while (1)
    {
	Edge *e = hash_table+hpos;
	if (e->v1 == (-1)) return NULL;
	if (e->v1 == v1 && e->v2 == v2) return e;

	hpos = (hpos+1)%n_hash;	// Look in next bucket
    }
}


