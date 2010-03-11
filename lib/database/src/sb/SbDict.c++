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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbDictEntry, SbDict
 |
 |   Author(s)		: Nick Thompson, Paul S. Strauss, Dave Immel
 |
 |   Notes:
 |	Dictionaries are maintained as external hash tables of
 |	SbDictEntry instances.
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbDict.h>

////////////////////////////////////////////
//
// This structure is used to implement makePList()
//

struct SbDictListThing {
    SbPList	*keyList;
    SbPList	*valueList;
};

//
////////////////////////////////////////////

//
// Constructor - empties all buckets in the hash table
//

SbDict::SbDict( int entries )
{
    tableSize=entries;
    buckets=new SbDictEntry *[tableSize];
    for (int i = 0; i < tableSize; i++)
	buckets[i] = NULL;
}

//
// Destructor - clears all buckets, frees up bucket table.
//

SbDict::~SbDict()
{
    clear();
    delete [] buckets;
}

//
// Adds one entry to the end of the PLists accessed through "data"
//

void
SbDict::addEntryToPLists(unsigned long key, void *value, void *data)
{
    SbDictListThing	*lists = (SbDictListThing *) data;

    lists->keyList->append((void *) key);
    lists->valueList->append(value);
}

//
// Calls given routine (passing value) for each entry in dictionary.
// The order of entries is not guaranteed to mean anything.
//

void
SbDict::applyToAll(void (*rtn)(unsigned long key, void *value) )
{
    int		i;
    SbDictEntry	*entry;

    // Call rtn for each entry in dict
    for (i = 0; i < tableSize; i++) {

	for (entry = buckets[i]; entry != NULL; entry = entry->next)
	    (*rtn)(entry->key, entry->value);
    }
}

//
// Calls given routine (passing value,data) for each entry in dictionary.
// The order of entries is not guaranteed to mean anything.
//

void
SbDict::applyToAll(void (*rtn)(unsigned long key, void *value, void *data),
		   void *data )
{
    int		i;
    SbDictEntry	*entry;

    // Call rtn for each entry in dict
    for (i = 0; i < tableSize; i++) {

	for (entry = buckets[i]; entry != NULL; entry = entry->next)
	    (*rtn)(entry->key, entry->value, data);
    }
}

//
// Removes all entries from dictionary.
//

void
SbDict::clear()
{
    int		i;
    SbDictEntry	*entry, *nextEntry;

    // Free up each entry in dict
    for (i = 0; i < tableSize; i++) {

	for (entry = buckets[i]; entry != NULL; entry = nextEntry) {
	    nextEntry = entry->next;
	    delete entry;
	}
	buckets[i] = NULL;
    }
}

//
// Enters a key,value pair into the dictionary. Overwrites entry and
// returns FALSE if key already exists.
//

SbBool
SbDict::enter(unsigned long key, void *value)
{
    SbDictEntry		*&entry = findEntry(key);

    if (entry == NULL) {
	entry = new SbDictEntry(key, value);
	entry->next = NULL;
	return TRUE;
    }
    else {
	entry->value = value;	// Overwrites old value
	return FALSE;
    }
}

//
// Finds entry with given key, setting value to point to value.
// Returns FALSE if no such entry.
//

SbBool
SbDict::find(unsigned long key, void *&value) const
{
    SbDictEntry		*&entry = findEntry(key);

    if (entry == NULL) {
	value = NULL;
	return FALSE;
    }
    else {
	value = entry->value;
	return TRUE;
    }
}

//
// Returns reference to entry with given key, or NULL if there is none.
//

SbDictEntry *&
SbDict::findEntry(unsigned long key) const
{
    SbDictEntry		**entry;

    entry = &buckets[key % tableSize];

    while (*entry != NULL) {
	if ((*entry)->key == key)
	    break;
	entry = &(*entry)->next;
    }
    return *entry;
}

//
// Makes two SbPLists, one for keys and the other for values.
// The order of entries is not guaranteed to mean anything.
//

void
SbDict::makePList(SbPList &keys, SbPList &values)
{
    SbDictListThing	lists;

    lists.keyList = &keys;
    lists.valueList = &values;

    keys.truncate(0);
    values.truncate(0);

    applyToAll(SbDict::addEntryToPLists, (void *) &lists);
}

//
// Removes the entry with the given key. Returns FALSE if no such entry.
//

SbBool
SbDict::remove(unsigned long key)
{
    SbDictEntry		*&entry = findEntry(key);
    SbDictEntry		*tmp;

    if (entry == NULL)
	return FALSE;
    else {
	tmp = entry;
	entry = entry->next;
	delete tmp;
	return TRUE;
    }
}
