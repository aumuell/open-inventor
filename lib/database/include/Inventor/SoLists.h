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
 |	This file contains the definitions of subclasses of SbPList for
 |	some of the specific Inventor pointer types so that lists of
 |	pointers can be created easily.
 |
 |   Classes:
 |	subclasses of SbPList:
 |		SoBaseList
 |		SoNodeList
 |		SoPathList
 |		SoEngineList
 |		SoTypeList
 |		SoDetailList
 |		SoPickedPointList
 |		SoFieldList
 |		SoEngineOutputList
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, David Mott,
 |                        Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LISTS_
#define  _SO_LISTS_

#include <Inventor/SbPList.h>
#include <Inventor/SoType.h>

class SoBase;
class SoDetail;
class SoEngine;
class SoEngineOutput;
class SoField;
class SoNode;
class SoPath;
class SoPickedPoint;

//////////////////////////////////////////////////////////////////////////////
//
// Subclasses of the SbPList class which hold lists of pointers of a
// specific type.
//
// Each contains:
//	A default constructor
//	A constructor taking an initial number of items in the list
//	An "append" function that adds a pointer to the end of the list
//	The index ([]) operator that returns the nth pointer in the list
//
//////////////////////////////////////////////////////////////////////////////

#if _COMPILER_VERSION>=710
#  pragma set woff 1375
#endif

class SoBaseList : public SbPList {
  public:
    SoBaseList();
    // C-api: name=createSize
    SoBaseList(int size);
    // C-api: end
    SoBaseList(const SoBaseList &l);
    // C-api: begin
    ~SoBaseList()			{ truncate(0); }

    // Add a pointer to the end of the list
    void		append(SoBase * ptr);

    // Insert given pointer in list before pointer with given index
    void		insert(SoBase *ptr, int addBefore);

    // Remove pointer with given index
    void		remove(int which);

    // Remove all pointers after one with given index, inclusive
    void		truncate(int start);

    // Copy a list, keeping all reference counts correct
    void		copy(const SoBaseList &l);
    // C-api: end
    SoBaseList &	operator =(const SoBaseList &l)
	{ copy(l) ; return *this; }
    // C-api: begin

    // Access an element of a list
    SoBase *		operator [](int i) const
	{ return ( (SoBase *) ( (*(const SbPList *) this) [i] ) ); }

    // Set an element of a list
    void		set(int i, SoBase *ptr);

    // Tells list whether to call ref() and unref() for bases in the
    // list when adding/removing them. The default value is TRUE.
    void		addReferences(SbBool flag)	{ addRefs = flag; }

  private:
    // If TRUE (the default), this refs and unrefs things in the list
    SbBool		addRefs;
};

class SoNodeList : public SoBaseList {
  public:
    SoNodeList()			: SoBaseList()	{}
    // C-api: name=createSize
    SoNodeList(int size)		: SoBaseList(size) {}
    // C-api: end
    SoNodeList(const SoNodeList &l)	: SoBaseList(l) {}
    // C-api: begin
    ~SoNodeList()			{ }

    void		append(SoNode * ptr)
	{ ((SoBaseList *) this)->append((SoBase *) ptr); }

    SoNodeList &	operator =(const SoNodeList &l)
	{ SoBaseList::copy(l) ; return *this; }

    SoNode *		operator [](int i) const
	{ return ( (SoNode *) ( (*(const SoBaseList *) this) [i] ) ); }
};

class SoPathList : public SoBaseList {
  public:
    SoPathList()			: SoBaseList()	{}
    // C-api: name=createSize
    SoPathList(int size)		: SoBaseList(size) {}
    // C-api: end
    SoPathList(const SoPathList &l)	: SoBaseList(l) {}
    // C-api: begin
    ~SoPathList()			{ }

    void		append(SoPath * ptr)
	{ ((SoBaseList *) this)->append((SoBase *) ptr); }

    SoPathList &	operator =(const SoPathList &l)
	{ SoBaseList::copy(l) ; return *this; }

    SoPath *		operator [](int i) const
	{ return ( (SoPath *) ( (*(const SoBaseList *) this) [i] ) ); }

    // Returns index of matching path in list, or -1 if not found
    int			findPath(const SoPath &path);

    // Sorts list in place based on (1) increasing address of head
    // node, then (2) increasing indices of children
    void		sort();

    // Given a sorted list, removes any path that (1) is a duplicate,
    // or (2) goes through a node that is the tail of another path
    void		uniquify();

  private:
    // Comparison method for path list sorting.
    static int		comparePaths(const void *p1Ptr, const void *p2Ptr);
};

class SoEngineList : public SoBaseList {
  public:
    SoEngineList()			: SoBaseList()	{}
    // C-api: name=createSize
    SoEngineList(int size)		: SoBaseList(size) {}
    // C-api: end
    SoEngineList(const SoEngineList &l)	: SoBaseList(l) {}
    // C-api: begin
    ~SoEngineList()			{ }

    void		append(SoEngine * ptr)
	{ ((SoBaseList *) this)->append((SoBase *) ptr); }

    SoEngineList &	operator =(const SoEngineList &l)
	{ SoBaseList::copy(l) ; return *this; }

    SoEngine *		operator [](int i) const
	{ return ( (SoEngine *) ( (*(const SoBaseList *) this) [i] ) ); }
};

class SoTypeList : public SbPList {
  public:
    SoTypeList()			: SbPList()	{}
    // C-api: name=createSize
    SoTypeList(int size)		: SbPList(size)	{}
    // C-api: end
    SoTypeList(const SoTypeList &l)	: SbPList(l)	{}
    // C-api: begin
    ~SoTypeList()			{ truncate(0); }

    // Add a typeId to the end of the list
    void		append(SoType typeId);

    // Returns index of given typeId in list, or -1 if not found
    int			find(SoType typeId) const;

    // Insert given typeId in list before typeId with given index
    void		insert(SoType typeId, int addBefore);

    // Access an element of a list
    SoType		operator [](int i) const;

    // Set an element of a list
    void		set(int i, SoType typeId);
};

// C-api: prefix=SoDtlList
class SoDetailList : public SbPList {
  public:
    SoDetailList()			: SbPList()	{}
    // C-api: name=createSize
    SoDetailList(int size)		: SbPList(size)	{}
    // C-api: end
    SoDetailList(const SoDetailList &l);
    // C-api: begin
    ~SoDetailList()			{ truncate(0); }

    // Add a detail to the end of the list
    void		append(SoDetail *detail)
	{ SbPList::append((void *) detail); }

    // Insert given detail in list before detail with given index
    void		insert(SoDetail *detail, int addBefore)
	{ SbPList::insert((void *) detail, addBefore); }

    // Remove all pointers after one with given index, inclusive,
    // deleting instances
    void		truncate(int start);

    // Copy a list, making copies of all detail instances
    void		copy(const SoDetailList &l);
    // C-api: end
    SoDetailList &	operator =(const SoDetailList &l)
	{ copy(l) ; return *this; }
    // C-api: begin

    // Access an element of a list
    SoDetail *		operator [](int i) const
	{ return ( (SoDetail *) ( (* (const SbPList *) this) [i] ) ); }

    // Set an element of a list, deleting old entry
    void		set(int i, SoDetail *detail);
};

// C-api: prefix=SoPickPtList
class SoPickedPointList : public SbPList {
  public:
    SoPickedPointList()			: SbPList()	{}
    // C-api: name=createSize
    SoPickedPointList(int size)		: SbPList(size)	{}
    // C-api: end
    SoPickedPointList(const SoPickedPointList &l);
    // C-api: begin
    ~SoPickedPointList()				{ truncate(0); }

    // Add a pickedPoint to the end of the list
    void		append(SoPickedPoint *pickedPoint)
	{ SbPList::append((void *) pickedPoint); }

    // Insert given pickedPoint in list before pickedPoint with given index
    void		insert(SoPickedPoint *pickedPoint, int addBefore)
	{ SbPList::insert((void *) pickedPoint, addBefore); }

    // Remove all pointers after one with given index, inclusive,
    // deleting instances
    void		truncate(int start);

    // Access an element of a list
    SoPickedPoint *	operator [](int i) const
	{ return ( (SoPickedPoint *) ( (* (const SbPList *) this) [i] ) ); }

    // Set an element of a list, deleting old entry
    void		set(int i, SoPickedPoint *pickedPoint);
};

class SoFieldList : public SbPList {
  public:
    SoFieldList()			: SbPList()	{}
    // C-api: name=createSize
    SoFieldList(int size)		: SbPList(size)	{}
    // C-api: end
    SoFieldList(const SoFieldList &l)	: SbPList(l)	{}
    // C-api: begin
    ~SoFieldList()			{ truncate(0); }

    // Add a Field to the end of the list
    void		append(SoField *field)
	{ SbPList::append((void *) field); }

    // Insert given field in list before field with given index
    void		insert(SoField *field, int addBefore)
	{ SbPList::insert((void *) field, addBefore); }

    // Access an element of a list
    SoField *		operator [](int i) const
	{ return ( (SoField *) ( (* (const SbPList *) this) [i] ) ); }

    // Set an element of a list
    void		set(int i, SoField *Field)
	{ (* (const SbPList *) this) [i] = (void *) Field; }

// Internal versions of [] that do not check for bounds:
  SoINTERNAL public:
    SoField *		get(int i) const
	{ return (SoField *)SbPList::get(i); }
};

class SoEngineOutputList : public SbPList {
  public:
    SoEngineOutputList()			: SbPList()	{}
    // C-api: name=createSize
    SoEngineOutputList(int size)		: SbPList(size)	{}
    // C-api: end
    SoEngineOutputList(const SoEngineOutputList &l)	: SbPList(l)	{}
    // C-api: begin
    ~SoEngineOutputList()			{ truncate(0); }

    // Add a EngineOutput to the end of the list
    void		append(SoEngineOutput *engineOutput)
	{ SbPList::append((void *) engineOutput); }

    // Insert given engineOutput in list before engineOutput with given index
    void		insert(SoEngineOutput *engineOutput, int addBefore)
	{ SbPList::insert((void *) engineOutput, addBefore); }

    // Access an element of a list
    SoEngineOutput *		operator [](int i) const
	{ return ( (SoEngineOutput *) ( (* (const SbPList *) this) [i] ) ); }

    // Set an element of a list
    void		set(int i, SoEngineOutput *engineOutput)
	{ (* (const SbPList *) this) [i] = (void *) engineOutput; }
};

//////////////////////////////////////////////////////////////////////////////

#if _COMPILER_VERSION>=710
#  pragma reset woff 1375
#endif

#endif /* _SO_LISTS_ */
