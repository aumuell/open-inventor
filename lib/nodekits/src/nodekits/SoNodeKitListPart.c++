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
 |      SoNodeKitListPart
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoMFName.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoMFName.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoCallbackAction.h>


SO_NODE_SOURCE(SoNodeKitListPart);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoNodeKitListPart::SoNodeKitListPart()
//
////////////////////////////////////////////////////////////////////////
{
    children = new SoChildList(this);
    // We use the NODE  constructor 
    SO_NODE_CONSTRUCTOR(SoNodeKitListPart);

    isBuiltIn = TRUE;

    // Create the container node field. The default is an SoGroup.
    SO_NODE_ADD_FIELD(containerTypeName,("Group"));
    SO_NODE_ADD_FIELD(childTypeNames,   (""));
    SO_NODE_ADD_FIELD(containerNode,    (NULL));

    // Turn off notification on this field.
    // We store the info as a field, but unless we 
    // turn off notification, everything takes forever.
    containerNode.enableNotify(FALSE);

    // By default, any type of node is permitted.
    // The first time a new childType is added, this first entry of 
    // 'SoNode' is removed and only types added to the list will be legal.
    childTypes.append( SoNode::getClassTypeId() );

    areTypesLocked = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoNodeKitListPart::~SoNodeKitListPart()
//
////////////////////////////////////////////////////////////////////////
{
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Locks the types of the container and child nodes.
//    Once called, the methods setContainerType and addChildType
//    no longer have any effect.
//
// Use: public

void
SoNodeKitListPart::lockTypes()
//
////////////////////////////////////////////////////////////////////////
{
    areTypesLocked = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns whether a type is legal as a child of the container.
//
// Use: public

SbBool
SoNodeKitListPart::isTypePermitted( SoType typeToCheck ) const
//
////////////////////////////////////////////////////////////////////////
{
    for ( int i = 0; i < childTypes.getLength(); i++ ) {
	if ( typeToCheck.isDerivedFrom( childTypes[i] ) )
	    return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns whether a type is legal as a child of the container.
//
// Use: public

SbBool
SoNodeKitListPart::isChildPermitted( const SoNode *child ) const
//
////////////////////////////////////////////////////////////////////////
{
    for ( int i = 0; i < childTypes.getLength(); i++ ) {
	if ( child->isOfType( childTypes[i] ) )
	    return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Passes the given string to the set() method of the container node.
//
// Use: public

void
SoNodeKitListPart::containerSet( const char *fieldDataString )
//
////////////////////////////////////////////////////////////////////////
{
    getContainerNode()->set( fieldDataString );
}

////////////////////////////////////////////////////////////////////////
//
// list methods.  Analogs the SoGroup methods
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This adds a child as the last one in the container.
//
// Use: public

void
SoNodeKitListPart::addChild( SoNode *child)  // child to add to group
//
////////////////////////////////////////////////////////////////////////
{
    if ( isChildPermitted( child ) ) {
	// Turn off notification while getting the container.
	// We'll be notifying when the child gets added, so there's
	// no reason to notify if the container is created as well.
	SbBool wasEn = enableNotify(FALSE);
	SoGroup *cont = getContainerNode();
	enableNotify(wasEn);

	cont->addChild(child);
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoNodeKitListPart::addChild",
	                   "--> Can\'t add child of type \"%s\" ",
	    		    child->getTypeId().getName().getString() );
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This inserts a child into the container so that it will have the given
//    index.
//
// Use: public

void
SoNodeKitListPart::insertChild( SoNode *child, int newChildIndex)
//
////////////////////////////////////////////////////////////////////////
{
    if ( isChildPermitted( child ) ) {
	getContainerNode()->insertChild(child, newChildIndex);
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoNodeKitListPart::insertChild",
			   "--> Can\'t insert child of type \"%s\" ",
	    		   child->getTypeId().getName().getString() );
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    returns the node of the given index from the container
//
// Use: public

SoNode *
SoNodeKitListPart::getChild(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return NULL;

    return ((SoGroup *)containerNode.getValue())->getChild(index);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the first index (starting with 0) of a child in container that
//    matches the given node pointer, or -1 if no such child is found.
//
// Use: public

int
SoNodeKitListPart::findChild( SoNode *child ) const
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return -1;

    return ((SoGroup *)containerNode.getValue())->findChild(child);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the first index (starting with 0) of a child in container that
//    matches the given node pointer, or -1 if no such child is found.
//
// Use: public

int
SoNodeKitListPart::getNumChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return 0;

    return ((SoGroup *)containerNode.getValue())->getNumChildren();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes the child with the given index from the container.
//
//    If the container is a switch, makes sure that the value of
//    whichChild doesn't turn 'illegal'
//
// Use: public

void
SoNodeKitListPart::removeChild( int index)
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return;

    SoGroup *grp = (SoGroup *) containerNode.getValue();
    grp->removeChild(index);

    // If the parent is a switch, make sure this doesn't 
    // screw it up...
    if ( grp->isOfType( SoSwitch::getClassTypeId() ) ){
	SoSwitch *sw = (SoSwitch *) grp;
	int swNum = sw->getNumChildren();
	if (sw->whichChild.getValue() >= swNum)
	    sw->whichChild.setValue(  swNum - 1 );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces child in container with given index with new child.
//
// Use: public

void
SoNodeKitListPart::replaceChild( int index, SoNode *newChild) 
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return;

    if ( isChildPermitted( newChild ) ) {
	((SoGroup *)containerNode.getValue())->replaceChild(index, newChild);
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoNodeKitListPart::replaceChild",
			   "--> Can\'t replace with child of type \"%s\"",
			   newChild->getTypeId().getName().getString() );
    }
#endif
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the node affect the state? Well, it all depends on the container
//    node.
//
// Use: public

SbBool
SoNodeKitListPart::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    if ( containerNode.getValue() == NULL )
	return FALSE;
    else
	return ( containerNode.getValue()->affectsState() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given nodekit into this instance.
//
// Use: protected, virtual

void
SoNodeKitListPart::copyContents(const SoFieldContainer *fromFC,
				SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    const SoNodeKitListPart *origList = (const SoNodeKitListPart *) fromFC;

    // Call the base class copy.
    SoNode::copyContents(fromFC, copyConnections);

    // Copy the child types.
    childTypes.truncate(0);
    for ( int i = 0; i < origList->childTypes.getLength(); i++ )
	childTypes.append( origList->childTypes[i] );

    // Copy the locked flag
    if ( origList->isTypeLocked() )
	lockTypes();

    // Right now, the value of this's containerNode matches the
    // value of origList's containerNode, because copying an SoSFNode merely
    // copies the pointer and ref's the node.  We need to make a new copy 
    // of origList's containerNode and put it in the field for this.
    SoNode *origContNode  = origList->containerNode.getValue();
    SoNode *newContNode = NULL;
    if ( origContNode != NULL ) {
	newContNode = origContNode->copy(copyConnections);
	containerNode.setValue( newContNode);
    }

    // If it exists, make newContNode be the first child of this
    if ( newContNode != NULL ) {
	if ( children->getLength() == 0 )
	    children->append( newContNode );
	else
	    children->insert( newContNode, 0 );
    }
}

////////////////////////////////////////////////////////////////////////
//
SoChildList *
SoNodeKitListPart::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
   return children; 
}

////////////////////////////////////////////////////////////////////////
//
// Returns type of container.
//
SoType
SoNodeKitListPart::getContainerType() const
//
////////////////////////////////////////////////////////////////////////
{
    return (SoType::fromName( containerTypeName.getValue() ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    [1] Turn off notification for this node
//    [2] Read the fields for the SoNodeKitListPart.
//    [3] Check that containerType is legal.
//    [4] Copy child types from name field into type list.
//        If none are specified, then allow anything by making
//        the first entry be 'SoNode'
//    [5] Check that containerNode is of correct type
//    [6] If containerNode exists, make it the first hidden child.
//    [7] Check that any children are of correct type, 
//    [8] Turn notification back on for node.
// 
// Use: protected

SbBool
SoNodeKitListPart::readInstance( SoInput *in, unsigned short flags )
//
////////////////////////////////////////////////////////////////////////
{
    SbBool readOK = TRUE;

    // [1] Turn off notification for this node
	SbBool saveNotify = enableNotify(FALSE);

    // [2] Read the fields for the SoNodeKitListPart.
    if ( !SoNode::readInstance( in, flags ) )
	readOK = FALSE;

    if ( readOK ) {
        // [3] Check that containerType is legal.
        const SoType ct = getContainerType();
	if (    !ct.isDerivedFrom( SoGroup::getClassTypeId())
	     || !ct.canCreateInstance() ) {
	    SoReadError::post(in,
	    "Given container type is not derived from Group or is an abstract class");
	    readOK = FALSE;
	}
	else {
	    // [4] Copy child types from name field into type list.
	    //     If none are specified, then allow anything by making
	    //     the first entry be 'SoNode'
	    childTypes.truncate(0);
	    if ( childTypeNames.isDefault() )
	        childTypes.append( SoNode::getClassTypeId() );
	    else {
		for ( int i = 0; i < childTypeNames.getNum(); i++ )
		    childTypes.append( SoType::fromName( childTypeNames[i] ) );
	    }

    	    // [5] Check that containerNode is of correct type
	    SoNode *contNode = containerNode.getValue();
	    if ( contNode != NULL && (contNode->isOfType( ct ) == FALSE ) ) {
		SoReadError::post(in, "Given container node is of incorrect type");
		readOK = FALSE;
	    }
	    else if ( contNode != NULL ) {

		// [6] If containerNode exists, make it the first hidden child.
		if (children->getLength() == 0)
		    children->append( contNode );
		else
		    children->insert( contNode, 0 );

		// [7] Check that any children are of correct type, 
		SoGroup *grp = (SoGroup *) contNode;
		for ( int i = grp->getNumChildren() - 1; i >= 0; i-- ) {
		    if (  !isChildPermitted( grp->getChild( i ) ) ) {
		    SoReadError::post(in, "Removing child number %d of illegal type", i);
			grp->removeChild( i );
		    }
		}
	    }
	}
    }

    // [8] Turn notification back on for node.
	enableNotify(saveNotify); 

    return readOK;
}
////////////////////////////////////////////////////////////////////////
//
// Gets rid of the old container and replaces it with a container of the
// new type.
// Checks first that the new container is a group.
//
void
SoNodeKitListPart::setContainerType( SoType newContainerType )
//
////////////////////////////////////////////////////////////////////////
{
    if ( isTypeLocked() ) {
#ifdef DEBUG
	SoDebugError::post("SoNodeKitListPart::setContainerType",
	"You can\'t change the type because the type lock has been turned on");
#endif
	return;
    }
    
    if ( newContainerType == getContainerType() )
	return;

    if ( !newContainerType.isDerivedFrom( SoGroup::getClassTypeId() ) )
	return;

    if ( newContainerType.canCreateInstance() == FALSE )
	return;

    // If necessary, create a new container node of the correct type:
    SoGroup *oldContainer = (SoGroup *) containerNode.getValue();
    SoGroup *newContainer = NULL;

    if (  oldContainer == NULL ||
	 !oldContainer->isOfType( newContainerType ) ) {

	newContainer = (SoGroup *) newContainerType.createInstance(); 
	newContainer->ref();

	    // copy children from oldContainer to new one.
	    if ( oldContainer != NULL ) {
		for (int i = 0; i < oldContainer->getNumChildren(); i++ ) 
		    newContainer->addChild( oldContainer->getChild(i) );
	    }
	
	    // replace the container in this nodes children list
	    int oldChildNum = children->find( oldContainer );
	    if ( oldChildNum == -1 )
	        children->insert( newContainer, 0 );
	    else
	        children->set( oldChildNum, newContainer );

	    containerNode.setValue( newContainer);

	newContainer->unref();
    }

    containerTypeName.setValue( newContainerType.getName() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a type to the child list. This means that nodes of this type
//    may now be added using addChild(), etc.
//
// Use: public

void
SoNodeKitListPart::addChildType( SoType typeToAdd )
//
////////////////////////////////////////////////////////////////////////
{
    if ( isTypeLocked() ) {
#ifdef DEBUG
	SoDebugError::post("SoNodeKitListPart::addChildType",
	"You can\'t change the type because the type lock has been turned on");
#endif
	return;
    }
    
    // If this is our first one, then truncate the childTypes to 0.
    // By default, (i.e., until we set the first one), the initial
    // entry is SoNode::getClassTypeId(), which allows any node to be
    // permitted.
    if ( childTypeNames.isDefault() )
	childTypes.truncate(0);

    // Add the type to the childTypes list if it's not there yet.
    if ( childTypes.find( typeToAdd ) == -1 ) {
	childTypes.append( typeToAdd );

	// Set the value of the corresponding entry in the
	// childTypeNames field.
	childTypeNames.set1Value(childTypes.getLength()-1,typeToAdd.getName());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a list of the allowable child types.
//    If nothing has been specified, any type of node is allowed.
//
// Use: public

const SoTypeList &
SoNodeKitListPart::getChildTypes() const
//
////////////////////////////////////////////////////////////////////////
{
    return childTypes;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the container node.
//
// Use: protected

SoGroup *
SoNodeKitListPart::getContainerNode()
//
////////////////////////////////////////////////////////////////////////
{ 
    if ( containerNode.getValue() != NULL )
	return ((SoGroup *) containerNode.getValue()); 
    else {
	SoType   contType = SoType::fromName( containerTypeName.getValue() );
	SoGroup *contNode = (SoGroup *) contType.createInstance(); 
	contNode->ref();

	// put contNode into this node's children list
	if (children->getLength() == 0)
	    children->append( contNode );
	else
	    children->insert( contNode, 0 );

	containerNode.setValue( contNode);
	contNode->unref();

	return contNode;
    }
}

void 
SoNodeKitListPart::doAction( SoAction *action )
{
    int	numIndices;
    const int *indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
    else
	children->traverse(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
// Use: protected

void
SoNodeKitListPart::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;

    // Only need to compute matrix if group is a node in middle of
    // current path chain or is off path chain (since the only way
    // this could be called if it is off the chain is if the group is
    // under a group that affects the chain).
    switch (action->getPathCode(numIndices, indices)) {

	case SoAction::NO_PATH:
	    break;

	case SoAction::IN_PATH:
	    children->traverse(action, 0, indices[numIndices - 1]);
	    break;

	case SoAction::BELOW_PATH:
	    break;

	case SoAction::OFF_PATH:
	    children->traverse(action);
	    break;
    }
}


// These functions implement all actions for nodekits.
void 
SoNodeKitListPart::callback( SoCallbackAction *action )
{ 
    SoNodeKitListPart::doAction( action );
}

void 
SoNodeKitListPart::GLRender( SoGLRenderAction *action )
{ SoNodeKitListPart::doAction( action ); }

void 
SoNodeKitListPart::getBoundingBox( SoGetBoundingBoxAction *action )
{ 
    SbVec3f     totalCenter(0,0,0);
    int         numCenters = 0;
    int         numIndices;
    const int	*indices;
    int         lastChild;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	lastChild = indices[numIndices - 1];
    else
	lastChild = children->getLength() - 1;

    for (int i = 0; i <= lastChild; i++) {
	children->traverse(action, i, i);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}
    }
    // Now, set the center to be the average:
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);

}

void 
SoNodeKitListPart::handleEvent( SoHandleEventAction *action )
{ SoNodeKitListPart::doAction( action ); }

void 
SoNodeKitListPart::pick( SoPickAction *action )
{ SoNodeKitListPart::doAction( action ); }

void 
SoNodeKitListPart::search( SoSearchAction *action )
{ 
    SoNode::search(action);
    SoNodeKitListPart::doAction( action );
}

