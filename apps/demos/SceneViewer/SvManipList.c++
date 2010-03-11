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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 * 
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:	SvManipList
 |
 |   Author(s):	David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbPList.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/manips/SoTransformManip.h>
#include "SvManipList.h"


typedef struct SvPathManipStuff {
    SoPath	     *selectionPath;
    SoTransformManip *manip;
    SoPath           *xfPath;
} SvPathManipStuff;

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
// Use: public
SvManipList::SvManipList()
//
////////////////////////////////////////////////////////////////////////
{
    list = new SbPList;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
// Use: public
SvManipList::~SvManipList()
//
////////////////////////////////////////////////////////////////////////
{
    delete list;
}

////////////////////////////////////////////////////////////////////////
//
// Use: public
int
SvManipList::getLength() const
//
////////////////////////////////////////////////////////////////////////
{
    return list->getLength();
}

////////////////////////////////////////////////////////////////////////
//
// Append adds the selectionPath/manip/xfPath stuff to the list.
// This ref()'s both the paths and the manip.
//
// Use: public
void
SvManipList::append(SoPath *selectionP, SoTransformManip *m, 
		    SoPath *xfP )
//
////////////////////////////////////////////////////////////////////////
{
    SvPathManipStuff *stuff = new SvPathManipStuff;
    
    stuff->selectionPath = selectionP;
    stuff->manip = m;
    stuff->xfPath = xfP;
    selectionP->ref();
    m->ref();
    xfP->ref();
    
    list->append(stuff);
}

////////////////////////////////////////////////////////////////////////
//
// Find locates the first selectionPath/manip/xfPath stuff whose 
// selectionPath is p, and returns the index in the list of that stuff.
//
// Use: public
int
SvManipList::find(const SoPath *p) const
//
////////////////////////////////////////////////////////////////////////
{
    int which = -1;
    
    for (int i = 0; (i < list->getLength()) && (which == -1); i++) {
	SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[i];
	if (*stuff->selectionPath == *p)
	    which = i;
    }
    
    return which;
}

////////////////////////////////////////////////////////////////////////
//
// Find locates the first selectionPath/manip/xfPath stuff whose manip is m,
// and returns the index in the list of that stuff.
//
// Use: public
int
SvManipList::find(const SoTransformManip *m) const
//
////////////////////////////////////////////////////////////////////////
{
    int which = -1;
    
    for (int i = 0; (i < list->getLength()) && (which == -1); i++) {
	SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[i];
	if (stuff->manip == m)
	    which = i;
    }
    
    return which;
}

////////////////////////////////////////////////////////////////////////
//
// findByXfPath locates the first selectionPath/manip/xfPath stuff whose 
// xfPath is p, and returns the index in the list of that stuff.
//
// Use: public
int
SvManipList::findByXfPath(const SoPath *p) const
//
////////////////////////////////////////////////////////////////////////
{
    int which = -1;
    
    for (int i = 0; (i < list->getLength()) && (which == -1); i++) {
	SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[i];
	if (*stuff->xfPath == *p)
	    which = i;
    }
    
    return which;
}

////////////////////////////////////////////////////////////////////////
//
// Remove removes the selectionPath/manip/xfPath stuff specified by 
// which index from the list. This unref()'s both paths and the manip.
//
// Use: public
void
SvManipList::remove(int which)
//
////////////////////////////////////////////////////////////////////////
{
    SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[which];
    
    stuff->selectionPath->unref();
    stuff->manip->unref();
    stuff->xfPath->unref();
    
    list->remove(which);
}

////////////////////////////////////////////////////////////////////////
//
// This returns the selectionPath in the selectionPath/manip/xfPath stuff 
// specified by which index.
//
// Use: public
SoPath *
SvManipList::getSelectionPath(int which) const
//
////////////////////////////////////////////////////////////////////////
{
    SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[which];
    return (stuff->selectionPath);
}

////////////////////////////////////////////////////////////////////////
//
// This returns the manip in the selectionPath/manip/xfPath stuff 
// specified by which index.
//
// Use: public
SoTransformManip *
SvManipList::getManip(int which) const
//
////////////////////////////////////////////////////////////////////////
{
    SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[which];
    return (stuff->manip);
}

////////////////////////////////////////////////////////////////////////
//
// This returns the xfPath of the manip in the 
// selectionPath/manip/xfPath stuff specified by which index.
//
// Use: public
SoPath *
SvManipList::getXfPath(int which) const
//
////////////////////////////////////////////////////////////////////////
{
    SvPathManipStuff *stuff = (SvPathManipStuff *) (*list)[which];
    return (stuff->xfPath);
}
