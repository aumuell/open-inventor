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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the SoV1CustomNode class, which is used to
 |      read in and convert old nodes into new nodes.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_V1CUSTOMNODE_
#define  _SO_V1CUSTOMNODE_

#include <Inventor/misc/upgraders/SoUpgrader.h>
#include <Inventor/fields/SoMFName.h>
#include <Inventor/fields/SoMFString.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoV1CustomNode
//
//////////////////////////////////////////////////////////////////////////////

class SoV1CustomNode : public SoUpgrader {

    SO_NODE_HEADER(SoV1CustomNode);

  public:
    // Fields
    SoMFName		className;	// Names in custom subclass hierarchy
    SoMFString		fields;		// Field data strings, 1 per subclass
    SoMFString		customData;	// Custom data strings, 1 per subclass

    // Default constructor
    SoV1CustomNode();

    // There are 9 steps to reading in and upgrading a custom node;
    // happily, subclasses only have to worry about three virtual
    // methods:
    //   createNewNode()       -- MUST be implemented
    //   interpretCustomData() -- optional
    //   setUpNewNode()        -- also optional
    // See comments below for more info...
    virtual SbBool	upgrade(SoInput *in, const SbName &refName,
				SoBase *&result);

  protected:
    // which is responsible for looking at the
    //   className field and creating an appropriate replacement node.

  SoINTERNAL public:
    static void		initClass();

  protected:
    // createNewNode should look at the className field and create an
    // appropriate replacement node.  The default method prints an
    // error message.  This is called after fields have been read, but
    // before children are read.
    virtual SoNode	*createNewNode();

    // This method is called to let subclass instances do whatever they
    // have to do based on the info in the customData field value with the
    // given index after the given instance has been read. The default
    // does nothing.  This is called after createNewNode, before
    // children have been read.
    virtual void	interpretCustomData(SoV1CustomNode *inst,
					    int index) const;

    // This method is passed the new node (created by the
    // createNewNode method, above).  It must copy the fields from the
    // V1 upgrader instance into the new node, and must move the
    // children from the upgrader to the new node.  The default method
    // does nothing.
    virtual void	setUpNewNode(SoNode *newNode);

    virtual ~SoV1CustomNode();
};

// Note: use the generic SO_NODE_HEADER/etc macros instead of the
// SO_CUSTOM macros.

#endif /* _SO_V1CUSTOMNODE_ */
