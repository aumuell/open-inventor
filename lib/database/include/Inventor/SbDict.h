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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines dictionaries. A dictionary maps some sort of
 |	unique key to a data pointer. Keys are unsigned long integers.
 |
 |   Classes:
 |	SbDictEntry, SbDict
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// C-api: end

#ifndef _SB_DICT_
#define _SB_DICT_

#include <Inventor/SbBasic.h>
#include <Inventor/SbString.h>
#include <Inventor/SbPList.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbDictEntry (internal to SB)
//
//  A dictionary is stored as a collection of entries, each of which
//  is an SbDictEntry. It contains the key and value for the entry and
//  a link to create lists of entries.
//
//////////////////////////////////////////////////////////////////////////////

SoINTERNAL class SbDictEntry {
  private:
    unsigned long		key;
    void *		value;

    SbDictEntry *	next;

    // Constructor
    SbDictEntry(unsigned long k, void *v)	{ key = k; value = v; };

friend class SbDict;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbDict
//
//  This is a dictionary mapping (unsigned long) integer keys to (void *) data
//  pointers.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SbDict {
  public:

    // Constructor
    SbDict( int entries = 251 );

    // Destructor
    ~SbDict();

    // Calls given routine (passing value) for each entry in dictionary.
    //  The order of entries is not guaranteed to mean anything.
    void	applyToAll(void (*rtn)(unsigned long key, void *value) );

    // Calls given routine (passing value,data) for each entry in dictionary.
    //  The order of entries is not guaranteed to mean anything.
    // C-api: name=applyToAllWithData
    void	applyToAll(void (*rtn)(unsigned long key, void *value, void *data), 
			   void *data );

    // Removes all entries from dictionary.
    void	clear();

    // Enters a key,value pair into the dictionary. Overwrites entry and
    //  returns FALSE if key already exists.
    SbBool	enter(unsigned long key, void *value);

    // Finds entry with given key, setting value to point to value.
    //  Returns FALSE if no such entry.
    SbBool	find(unsigned long key, void *&value) const;

    // Makes two SbPLists, one for keys and the other for values.
    //  The order of entries is not guaranteed to mean anything.
    void	makePList(SbPList &keys, SbPList &values);

    // Removes the entry with the given key. Returns FALSE if no such entry.
    SbBool	remove(unsigned long key);

  private:
    // Entries are stored as an external hash table of SbDictEntry instances.
    int			tableSize;			// Entries in table
    SbDictEntry *	*buckets;			// Hash table

    SbDictEntry *&	findEntry(unsigned long key) const;
    static void		addEntryToPLists(unsigned long key, void *value, void *data);
};

#endif /* _SB_DICT_ */
