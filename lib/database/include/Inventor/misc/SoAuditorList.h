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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the definition of the internal
 |	SoAuditorList class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_AUDITOR_LIST_
#define _SO_AUDITOR_LIST_

#include <Inventor/SbPList.h>
#include <Inventor/misc/SoNotification.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAuditorList
//
//  SoAuditorList class. This class maintains a list of instances that
//  audit (receive notification from) an SoBase or SoField. Each entry
//  in the list consists of a pointer to the auditor (base or field)
//  instance and a type code of type SoNotRec::Type.
//
//  The type of the auditor object pointer depends on the type code,
//  as follows:
//
//	Type code:	Auditor object pointer:
//
//	CONTAINER	The SoFieldContainer containing the field
//	PARENT		The parent node
//	SENSOR		The SoDataSensor instance
//	FIELD		The destination field instance
//	ENGINE		The destination field instance
//
//  Since there are two entries in the underlying SbPList for each
//  auditor, some of these methods have to do some fancy footwork.
//
//////////////////////////////////////////////////////////////////////////////

#if _COMPILER_VERSION>=710
#  pragma set woff 1375
#endif

SoINTERNAL class SoAuditorList : private SbPList {
  public:

    // Constructor and destructor.
    SoAuditorList();
    ~SoAuditorList();

    // Adds an auditor of the given type to the list
    void		append(void *auditor, SoNotRec::Type type);

    // Sets auditor and type for given index
    void		set(int index, void *auditor, SoNotRec::Type type);

    // Finds an auditor in the list, returning the index or -1 if not found
    int			find(void *auditor, SoNotRec::Type type) const;

    // Returns object or type for given index
    void *		getObject(int index) const;
    SoNotRec::Type	getType(int index) const;

    // Removes an auditor from the list
    void		remove(int index);
    void		remove(void *auditor, SoNotRec::Type type)
	{ remove(find(auditor, type)); }

    // Returns number of auditors in list
    int			getLength() const;

    // Propagates notification to all auditors in list
    void		notify(SoNotList *list);

  private:
    // Propagates notification to indexed auditor
    void		notify1(SoNotList *list, int index);
};

#if _COMPILER_VERSION>=710
#  pragma reset woff 1375
#endif

#endif /* _SO_AUDITOR_LIST_ */
