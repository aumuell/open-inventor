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

/////////////////////////////////////////////////////////////////////////////
//
// IfHasher class: Uses a hash table to maintain a list of unique 2- or
// 3-dimensional vectors. Adding a vector to the list checks to see if
// an identical vector has already been added and returns the old
// index, if so.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_HASHER_
#define  _IF_HASHER_

#include <Inventor/SbLinear.h>

class SbDict;
class SoMFVec2f;
class SoMFVec3f;

class IfHasher {

  public:
    // Since this works with either 2- or 3-dimensional vectors, there
    // are two constructors. Each takes a field of the correct type in
    // which to store the results, minimum values and scale factors
    // used to set up a reasonable hash function, and the maximum
    // number of vectors to be added. The minimum values and scale
    // factors are used to bring the range of possible values of the
    // vectors into the interval (0,1) in all dimensions. Note that
    // nobody else should modify or access the passed field while the
    // IfHasher instance is active, until finish() is called.
    IfHasher(SoMFVec2f *field, int maxNumVectors,
	   const SbVec2f &min, const SbVec2f &scale);
    IfHasher(SoMFVec3f *field, int maxNumVectors,
	   const SbVec3f &min, const SbVec3f &scale);

    // Destructor
    ~IfHasher();

    // These add a vector to the field, or re-use an existing value in
    // it. The index of the value (new or old) is returned. Room is
    // made in the field, if necessary.
    int			addVector(const SbVec2f &newVector);
    int			addVector(const SbVec3f &newVector);

    // Finishes up
    void		finish();

  private:
    // These structures are stored in lists in the hash table
    // (external hashing) 
    struct HashEntry {
	int		index;		// Index into field
	HashEntry	*next;		// For creating hash lists
    };

    int			dimension;	// Dimension of vectors (2 or 3)
    SoMFVec2f		*field2;	// Given field
    SoMFVec3f		*field3;	// Given field
    SbVec2f		*vectors2;	// Pointer into field values 
    SbVec3f		*vectors3;	// Pointer into field values 
    SbVec3f		hashMin;	// Minimum coord values
    SbVec3f		hashScale;	// Hash scale factors (2 or 3 used)
    SbDict		*vectorDict;	// Dictionary storing vectors
    int			maxNum;		// Max number of vectors
    HashEntry		*entries;	// Array of HashEntry instances
    int			curEntry;	// Next entry to use

    // Stuff common to both constructors
    void		commonConstructor();

    // Adds the given vector to the hash table, if it is not already
    // there. Returns TRUE if the vector was added.
    SbBool		addIfNotThere(const float *newVector, int &index);

    // Makes room in the entries list and field for more values
    void		expandSize();

    // Hash function for finding duplicate vectors
    uint32_t		hashVector(const float *v);

    // Returns TRUE if the given vector matches the given existing vector
    SbBool		sameVector(const float *vector, int index);
};

#endif /* _IF_HASHER_ */
