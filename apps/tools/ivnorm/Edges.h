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

struct Edge
{
    Edge() { v1 = v2 = (-1); }
    int32_t v1, v2;
    FaceList faces;
};

class EdgeDict
{
  public:
    EdgeDict(int initial_size = 500);
    ~EdgeDict();

    void Add(Face *, int32_t v1, int32_t v2);

    void OtherFaces(Face *, int32_t v1, int32_t v2, FaceList &result);

  private:

    // Add to hash table.  Will create an entry if necessary.
    Edge *Enter(int32_t v1, int32_t v2);

    // Search for an edge; returns NULL if not found
    Edge *Find(int32_t v1, int32_t v2);

    int HashFunc(int32_t v1, int32_t v2);
    void ReHash(int new_size);

    Edge *hash_table;
    int n_hash;	// length of hash table
    int n_edges;	// # of edges added to hash table
};
