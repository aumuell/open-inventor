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

#include <Inventor/SbDict.h>
#include <Inventor/SbPList.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>

#include "IfAssert.h"
#include "IfHasher.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructors for 2- or 3-dimensional vectors.
//
/////////////////////////////////////////////////////////////////////////////

IfHasher::IfHasher(SoMFVec2f *field, int maxNumVectors,
	       const SbVec2f &min, const SbVec2f &scale)
{
    dimension = 2;

    field2 = field;
    field3 = NULL;

    hashMin[0] = min[0];
    hashMin[1] = min[1];
    hashMin[2] = 0.0;

    hashScale[0] = scale[0];
    hashScale[1] = scale[1];
    hashScale[2] = 1.0;

    maxNum = maxNumVectors;

    commonConstructor();
}

IfHasher::IfHasher(SoMFVec3f *field, int maxNumVectors,
	       const SbVec3f &min, const SbVec3f &scale)
{
    dimension = 3;

    field2 = NULL;
    field3 = field;

    hashMin = min;
    hashScale = scale;

    maxNum = maxNumVectors;

    commonConstructor();
}

/////////////////////////////////////////////////////////////////////////////
//
// Stuff common to both constructors.
//
/////////////////////////////////////////////////////////////////////////////

void
IfHasher::commonConstructor()
{
    // Create an array of HashEntry instances
    entries = new HashEntry[maxNum];
    curEntry = 0;

    // Make room in the field and begin editing
    if (dimension == 2) {
	field2->setNum(maxNum);
	vectors2 = field2->startEditing();
	vectors3 = NULL;
    }
    else {
	field3->setNum(maxNum);
	vectors3 = field3->startEditing();
	vectors2 = NULL;
    }

    // Set up the dictionary
    vectorDict = new SbDict(1235);

    // Multiply the scale values by some arbitrary numbers to make
    // the hash function more random
    hashScale[0] *=    233;
    hashScale[1] *=  72091;
    hashScale[2] *= 453451;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfHasher::~IfHasher()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// These add a vector to the list, returning its index. Room is
// made in the field, if necessary.
//
/////////////////////////////////////////////////////////////////////////////

int
IfHasher::addVector(const SbVec2f &newVector)
{
    ASSERT(vectorDict != NULL);
    ASSERT(dimension == 2);

    // See if we need to add this one, or is it already there
    int index;
    SbBool notThere = addIfNotThere(newVector.getValue(), index);

    // If it's not there, we need to add it to the field
    if (notThere)
	vectors2[index] = newVector;

    return index;
}

int
IfHasher::addVector(const SbVec3f &newVector)
{
    ASSERT(vectorDict != NULL);
    ASSERT(dimension == 3);

    // See if we need to add this one, or is it already there
    int index;
    SbBool notThere = addIfNotThere(newVector.getValue(), index);

    // If it's not there, we need to add it to the field
    if (notThere)
	vectors3[index] = newVector;

    return index;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds the given vector to the hash table, if it is not already
// there. Returns TRUE if the vector was added.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfHasher::addIfNotThere(const float *newVector, int &index)
{
    ASSERT(curEntry < maxNum - 1);

    // Find the hash key
    uint32_t key = hashVector(newVector);

    // See if there already is a list of vectors with the same key
    void *list;
    if (vectorDict->find(key, list)) {

	// If so, look through the list for an exact match
	HashEntry *entry, *prevEntry = NULL;
	for (entry = (HashEntry *) list; entry != NULL; entry = entry->next) {

	    ASSERT(entry->index < curEntry);

	    // If the vectors are the same, re-use the old one and stop
	    if (sameVector(newVector, entry->index)) {
		index = entry->index;
		return FALSE;
	    }

	    prevEntry = entry;
	}

	// If there were no matches, add the new vector to the end of
	// the list
	ASSERT(prevEntry != NULL);
	entry = &entries[curEntry];
	entry->index = curEntry;
	entry->next = NULL;
	prevEntry->next = entry;
    }

    // If there were no entries in the dictionary, start one
    else {
	HashEntry *entry = &entries[curEntry];
	entry->index = curEntry;
	entry->next = NULL;
	vectorDict->enter(key, entry);
    }

    index = curEntry++;
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Hash function for finding duplicate vectors.
//
/////////////////////////////////////////////////////////////////////////////

uint32_t
IfHasher::hashVector(const float *v)
{
    float sum = 0.0;
    for (int i = 0; i < dimension; i++)
	sum += (v[i] - hashMin[i]) * hashScale[i];
    return (int32_t) sum;
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the given vector matches the given existing vector.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfHasher::sameVector(const float *vector, int index)
{
    if (dimension == 2)
	return (vector[0] == vectors2[index][0] &&
		vector[1] == vectors2[index][1]);
    else
	return (vector[0] == vectors3[index][0] &&
		vector[1] == vectors3[index][1] &&
		vector[2] == vectors3[index][2]);
}

/////////////////////////////////////////////////////////////////////////////
//
// Finishes up.
//
/////////////////////////////////////////////////////////////////////////////

void
IfHasher::finish()
{
    // Stop editing and set the field to contain the correct number of
    // values
    if (dimension == 2) {
	field2->finishEditing();
	field2->setNum(curEntry);
    }
    else {
	field3->finishEditing();
	field3->setNum(curEntry);
    }

    // Get rid of the dictionary and the HashEntry instances
    delete vectorDict;
    delete [] entries;

    // Make sure nobody uses this again
    vectorDict = NULL;
}
