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
 |   $Revision: 1.3 $
 |
 |   Description:
 |	This contains the definition of the SbPList generic pointer
 |	list class. An SbPList is a list of (void *) pointers that
 |	allows easy insertion, removal, and other operations.
 |
 |	The SbIntList class allows lists of integers to be created.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SB_PLIST_
#define _SB_PLIST_

#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbPList
//
//  List of generic (void *) pointers. SbPList allows random access,
//  insertion, and removal.
//
//////////////////////////////////////////////////////////////////////////////

#if _COMPILER_VERSION>=710
#  pragma set woff 1375
#endif

class SbPList {
  public:

    // Constructor
    SbPList();

    // Constructor allocates enough room for the given number of pointers
    // C-api: name=createN
    SbPList(int initSize);

    // Constructor for copying
// C-api: end
    SbPList(const SbPList &pl);

// C-api: begin
    // Destructor
    ~SbPList();

    // Adds given pointer to end of list
    void	append(void * ptr)
	{ if (nPtrs + 1 > ptrsSize) expand(nPtrs + 1);
	  ptrs[nPtrs++] = ptr; }

    // Returns index of given pointer in list, or -1 if not found
    int		find(const void *ptr) const;

    // Inserts given pointer in list before pointer with given index
    void	insert(void *ptr, int addBefore);

    // Removes pointer with given index
    void	remove(int which);

    // Returns number of pointers in list
    // C-api: name=getLen
    int		getLength() const		{ return (int) nPtrs;	}

    // Removes all pointers after one with given index, inclusive
    // ??? should really be an error check in here
    void	truncate(int start)
	{ nPtrs = start; }

    // Copy a PList
    void	copy(const SbPList &pl);
// C-api: end

    SbPList &	operator =(const SbPList &pl)	{ copy(pl); return *this; }

// C-api: begin
    // Returns pointer with given index
    void *&	operator [](int i) const
    	{ if (i >= nPtrs) grow(i); return ptrs[i]; }

    // equality tests
    int		operator ==(const SbPList &pl) const
	{ return pl.nPtrs == nPtrs ? compare(pl) : FALSE; }
    int		operator !=(const SbPList &pl) const
	{ return pl.nPtrs == nPtrs ? ! compare(pl) : TRUE; }
  
// Internal versions of [] that do not check for bounds:
  SoINTERNAL public:
    void *	get(int i) const { return ptrs[i]; }
    void	set(int i, void *j) { ptrs[i] = j; }

  private:

    // NOTE: this must be called only if the number of elements in the two
    // lists is the same, otherwise badness could result
    int		compare(const SbPList &pl) const;

    void **	ptrs;		// The collection of pointers
    int		nPtrs;		// Number of pointers used
    int		ptrsSize;	// Number of pointers allocated

    // There are three(!) methods to expand the list.  grow() is used
    // when space is dynamically created, and needs to be initialized
    // to NULL:
    void	grow(int max) const;

    // setSize is used by grow and in other places internally where we
    // know that nothing needs to be initialized to NULL.
    void	setSize(int size)
	{ if (size > ptrsSize) expand(size); nPtrs = size; }

    // expand is the lowest level routine.  It just reallocates the
    // array and copies over the old values.
    void	expand(int size);
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbIntList
//
//  List of generic (void *) pointers. SbIntList allows random access,
//  insertion, and removal.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: end

SoINTERNAL class SbIntList : public SbPList {

  public:
    // Constructors, similar to SbPList
    SbIntList()					{}
    // C-api: name=createN
    SbIntList(int initSize) : SbPList(initSize) {}

    void	append(int integer)
	{ ((SbPList *) this)->append((void *) (unsigned long) integer); }

    // Returns index of given integer in list, or -1 if not found
    int		find(int integer)
	{ return ((SbPList *) this)->find((void *) (unsigned long) integer); }

    // Inserts given integer in list before integer with given index
    void	insert(int integer, int addBefore)
	{ ((SbPList *) this)->insert((void *) (unsigned long) integer, addBefore); }

    int &	operator [](int i) const
#if (_MIPS_SZPTR==64) && (_MIPS_SZINT==32)
	// An ugly cast that makes sure that when we cast from void* to
	// int&, we get the rightmost four bytes, rather than the upper
	// bytes.  
	{ return ((int &)*(((char*)&((*(const SbPList *) this) [i]) ) + 4 ));}

#elif 1 || ((_MIPS_SZPTR==32) && (_MIPS_SZINT==32))
	{ return ( (int &) ( (*(const SbPList *) this) [i] ) ); }
#else
#	error "Error in SbPList:  Don't know how to cast void* to int!"
#endif
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec3fList
//
//  List of vectors. Probably mostly used for vertices of polygons
//
//////////////////////////////////////////////////////////////////////////////

SoINTERNAL class SbVec3fList : public SbPList {

  public:
     SbVec3fList() {}
    ~SbVec3fList();

    void	append(SbVec3f *vec)
	{ ((SbPList *) this)->append((void *) new SbVec3f(vec->getValue())); }

    // Inserts given integer in list before integer with given index
    void	insert(SbVec3f *vec, int addBefore)
	{ ((SbPList *) this)->insert((void *) new SbVec3f(vec->getValue()),
		addBefore); }

    SbVec3f *   operator [](int i) const
	{ return ( (SbVec3f *) ( (*(const SbPList *) this) [i] ) ); }
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbStringList
//
//  A list of strings.  This list is used to list classes associated with 
//  a specific error.
//
//////////////////////////////////////////////////////////////////////////////

class SbString;

SoEXTENDER class SbStringList : public SbPList {
  public:
    void	append(SbString *string)
	{ ((SbPList *) this)->append((void *) string); }

    int 	 find(SbString *string)
	{ return ((SbPList *) this)->find((void *) string); }

    void	insert(SbString *string, int addBefore)
	{ ((SbPList *) this)->insert((void *) string, addBefore); }

    SbString *&	operator [](int i) const
	{ return ( (SbString *&) ( (*(const SbPList *) this) [i] ) ); }
};

// C-api: begin

#if _COMPILER_VERSION>=710
#  pragma reset woff 1375
#endif

#endif /* _SB_PLIST_ */
