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
 |      Defines the SoNodeKitListPart class. 
 |      It behaves sort of like a group to the outsider, but it only allows
 |      certain types of nodes to be put beneath it.
 | 
 |      It contains hidden children, and is not derived from SoGroup.
 |      The addChild(), insertChild(), etc. methods all check that the
 |      new node is allowable beneath it.
 |
 |      The container node sits beteen the SoNodeKitListPart and the children.
 |      It's type is taken in the constructor, and must be derived from 
 |      SoGroup. By making the container an SoSeparator, SoSwitch, etc.,
 |      the SoNodeKitList can be made to encase its set of children in
 |      the appropriate behavior.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_NODE_KIT_LIST_PART_
#define  _SO_NODE_KIT_LIST_PART_

#include <Inventor/misc/SoChildList.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoMFName.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodekits/SoNodekitParts.h>
#include <Inventor/nodekits/SoBaseKit.h>

class SoGroup;

class SoGetMatrixAction;
class SoGLRenderAction;
class SoGetBoundingBoxAction;
class SoHandleEventAction;
class SoSearchAction;
class SoSearchAction;
class SoCallbackAction;

////////////////////////////////////////////////////////////////////
//    Class: SoNodeKitListPart
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoNkitListPart
class SoNodeKitListPart : public SoNode {

    SO_NODE_HEADER(SoNodeKitListPart);

  public:

    // Constructor
    SoNodeKitListPart();

    // Returns the type of the contaner node. Default containerType is SoGroup.
    SoType	   getContainerType() const;
    void           setContainerType( SoType newContainerType );

    // By default, any type of node is allowed to be a child.
    // Once addChildType() is called, all children must belong to 
    // at least one of the types in the list.
    const SoTypeList &getChildTypes() const;
    void             addChildType( SoType typeToAdd );

    SbBool        isTypePermitted( SoType typeToCheck ) const;
    SbBool        isChildPermitted( const SoNode *child ) const;

    // Sends a string to the 'set' method on the container node.
    // This is how you can set the value of a switch node if the container
    // node is an SoSwitch, for example.
    void containerSet( const char *fieldDataString );


    // After this method is called, the types are locked in place.
    // Once called, the methods setContainerType() and addChildType()
    // will have no effect.
    // The types can not be unlocked. This gives the original creator of the
    // node a chance to "set it and forget it"
    void   lockTypes(); 
    SbBool isTypeLocked() const { return areTypesLocked; }


    // Child operations 
    void		addChild(SoNode *child);
    void		insertChild(SoNode *child, int childIndex);
    SoNode		*getChild(int index) const;
    int			findChild(SoNode *child) const;
    int			getNumChildren() const;
    void		removeChild(int index);
    // C-api: name=removeChildNode
    void		removeChild(SoNode *child)
		{ removeChild( findChild(child));	}
    void		replaceChild(int index, SoNode *newChild);
    // C-api: name=replaceChildNode
    void		replaceChild( SoNode *oldChild, SoNode *newChild)
		{ replaceChild(findChild(oldChild), newChild); }

    // Depending on the type of the container, this may
    // or may not affect the state.
    virtual SbBool affectsState() const;


  SoEXTENDER public:

    // These functions implement all actions for nodekits.
    virtual void doAction( SoAction *action );
    virtual void callback( SoCallbackAction *action );
    virtual void GLRender( SoGLRenderAction *action );
    virtual void getBoundingBox( SoGetBoundingBoxAction *action );
    virtual void getMatrix(SoGetMatrixAction *action );
    virtual void handleEvent( SoHandleEventAction *action );
    virtual void pick( SoPickAction *action );
    virtual void search( SoSearchAction *action );

  SoINTERNAL public:
    static void initClass();

    // Returns pointer to children
    virtual SoChildList *getChildren() const;

  protected:

    SoGroup *getContainerNode();

    // Reads in from file. Takes care of setting parts and stuff.
    virtual SbBool readInstance(SoInput *in, unsigned short flags);

    SoChildList *children;

    // Copies the contents of the given nodekit into this instance
    virtual void	copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

  protected:
    virtual ~SoNodeKitListPart();

  private:
    SoSFName containerTypeName;
    SoMFName childTypeNames;
    SoSFNode containerNode;

    SoTypeList childTypes;

    SbBool  areTypesLocked;

    friend class SoBaseKit;
};

#endif  /* _SO_NODE_KIT_LIST_PART_ */
