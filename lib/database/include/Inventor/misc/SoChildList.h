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
 |	This file contains the definition of the extender SoChildList
 |	class.
 |	  
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_CHILD_LIST_
#define _SO_CHILD_LIST_

#include <Inventor/SoLists.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoChildList
//
//  This class maintains a list of child nodes for any node. It allows
//  children to be added, removed, replaced, and accessed. The SoGroup
//  class has an instance of this class and provides public methods
//  for dealing with children. Other classes can also have an instance
//  of this class and may choose to publicize whatever methods they
//  wish for dealing with them. The SoNode::getChildren() method
//  returns the child list for any node class.
//
//  SoChildList automatically maintains the auditor list for parent
//  nodes. That is, when a child is added to the list, the parent node
//  is automatically added as a parent auditor of the child. To make
//  this possible, the parent node must be passed in to the
//  constructors of the SoChildList class.
//
//////////////////////////////////////////////////////////////////////////////

class SoAction;

SoEXTENDER class SoChildList : public SoNodeList {

  public:
    // Constructors and destructor.
    SoChildList(SoNode *parentNode);
    SoChildList(SoNode *parentNode, int size);
    SoChildList(SoNode *parentNode, const SoChildList &l);
    ~SoChildList();

    // All of these override the standard SoBaseList methods so they
    // can maintain auditors correctly.
    void		append(SoNode * child);
    void		insert(SoNode *child, int addBefore);
    void		remove(int which);
    void		truncate(int start);
    void		copy(const SoChildList &l);
    void		set(int i, SoNode *child);

    // Traverses all children to apply action. Stops if action's
    // termination condition is reached
    void		traverse(SoAction *action)
	{ traverse(action, 0, getLength() - 1); }

    // Traverses just one child
    void		traverse(SoAction *action, int childIndex)
	{ traverse(action, childIndex, childIndex); }

    // Traverses all children between two indices, inclusive. Stops if
    // action's termination condition is reached.
    void		traverse(SoAction *action,
				 int firstChild, int lastChild);

  SoINTERNAL public:
    // SoPath calls these to be notified of changes in scene graph
    // topology:
    void addPathAuditor(SoPath *p) { auditors.append(p); }
    void removePathAuditor(SoPath *p) 
		{ auditors.remove(auditors.find(p)); }

  private:
    SoNode		*parent;
    // This is a PList and not a PathList because PathList ref()s the
    // paths it contains, and that screws up Path reference counting.
    SbPList		auditors;
};

#endif /* _SO_CHILD_LIST_ */
