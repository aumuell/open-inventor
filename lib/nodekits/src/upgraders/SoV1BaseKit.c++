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
 |      SoV1BaseKit
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>

#include <Inventor/misc/upgraders/SoV1BaseKit.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/errors/SoDebugError.h>


// Defines for printing out file data for nodekits
#define SO_V1_BASEKIT_FILEDATA_HEADER "partName/childNum pairs [ "
#define SO_V1_BASEKIT_FILEDATA_FOOTER "]"

// Define the required type id and name inquiry methods
SO_NODE_SOURCE(SoV1BaseKit);

// Define the required catalog inquiry methods
SO_V1_SUBKIT_CATALOG_VARS(SoV1BaseKit);
SO_V1_SUBKIT_CATALOG_METHODS(SoV1BaseKit);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1BaseKit::SoV1BaseKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1BaseKit);

    if (SO_NODE_IS_FIRST_INSTANCE() ) {

	// Initialize children catalog and add entries to it
	// NOTE: for all classes other than the SoV1BaseKit class, the
	//       macro SO_V1_SUBKIT_INHERIT_CATALOG is used.
	// the nodekitCatalog is made from scratch ONLY in the SoV1BaseKit class
	nodekitCatalog = new SoV1NodekitCatalog();
	SO_V1_SUBKIT_ADD_CATALOG_ENTRY("this",SoV1BaseKit, "", "", FALSE);
	SO_V1_SUBKIT_ADD_CATALOG_ENTRY("label",SoLabel, "this", "", TRUE );
    }

    if ( getNodekitCatalog() != NULL )
        nodekitPartsList = new SoV1NodekitParts( this );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoV1BaseKit::~SoV1BaseKit()
//
////////////////////////////////////////////////////////////////////////
{
    delete nodekitPartsList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: protected

SoNode *
SoV1BaseKit::getAnyPart( const SbName &partName, const SoType &partType, 
			SbBool makeIfNeeded, SbBool leafCheck, 
			SbBool publicCheck, SoPath * /* inheritancePath */ )
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *theNode = nodekitPartsList->getAnyPart( partName, makeIfNeeded, 
						leafCheck, publicCheck );

    // check that the node we are giving back is derived from
    // partType, so that it is safe for the recipient of 'theNode'
    // to treat it as being of that type
    if ( theNode != NULL 
	&& !theNode->isOfType( partType )) {
#ifdef DEBUG
	SoDebugError::post("SoV1BaseKit::getAnyPart",
	    "the node you asked for, %s, is not of the type you asked for",
			   partName.getString() );
#endif
	return NULL;
    }
    return theNode;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: public

SoNode *
SoV1BaseKit::getPart( const SbName &partName, const SoType &partType, 
		    SbBool makeIfNeeded, 
		    SoPath *inheritancePath )
//
////////////////////////////////////////////////////////////////////////
{
    // the fourth argument, leafCheck and publicCheck are TRUE, because we 
    // don't ordinarily return parts unless they are public leaves.
    return ( getAnyPart( partName, partType, makeIfNeeded, TRUE, TRUE,
			 inheritancePath ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: protected

SbBool
SoV1BaseKit::setAnyPart( const SbName &partName, SoNode *from, SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    if ( from )
        from->ref();
    SbBool answer = nodekitPartsList->setAnyPart( partName, from, anyPart );
    if ( from )
        from->unref();
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Check to see if the given node is derived from any of the typeIds
//    in the given typeList
//
// Use: public

SbBool
SoV1BaseKit::childOkInList(const SoNode *child,
		         const SbPList &listToCheck ) const
//
////////////////////////////////////////////////////////////////////////
{
    for( int i = 0; i < listToCheck.getLength(); i++ ) {
	if ( child->isOfType( *((SoType *) listToCheck[i] ) ) )
	    return TRUE;
    }

    return FALSE;
}

#define COMMENT_CHAR	'#'


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates the nodekitPartsList list.
//    Should be called once during the constructor for each instance.
//
//
//
// Use: protected

void
SoV1BaseKit::createNodekitPartsList()
//
////////////////////////////////////////////////////////////////////////
{
    if ( getNodekitCatalog() != NULL ) {
	SoV1NodekitParts *oldParts = nodekitPartsList;
        nodekitPartsList = new SoV1NodekitParts( this, oldParts );
	if (oldParts)
	    delete oldParts;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Interprets the customData string and checks the structure of the given
//    node based on those contents.
//
// Use: private

void
SoV1BaseKit::interpretCustomData(SoV1CustomNode *inst, // Node to interpret for
			     int index ) const 	 // Index into the customData
						 // field for finding the
						 // string to interpret.
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if ( !inst->isOfType( SoV1BaseKit::getClassTypeId() )) {
	SoDebugError::post("SoV1BaseKit::interpretCustomData",
	    "Given node is not a nodekit.");
	return;
    }
#endif

    // get the catalog and parts list
    SoV1BaseKit *theKit = (SoV1BaseKit *) inst;

    // only do this if 'index' is for the class SoV1BaseKit...
    if ( theKit->className.getNum() <= index  ||
	 theKit->className[index] != "BaseKit" )
	    return;

    SoV1NodekitParts *nkp = theKit->nodekitPartsList;
    const SoV1NodekitCatalog *theCat = theKit->getNodekitCatalog();

#ifdef DEBUG
    // make sure that 'inst' is listed as the part 'this' in its own
    // nodekitPartsList list.
    if ( inst != nkp->nodeList[0] ) {
	SoDebugError::post("SoV1BaseKit::interpretCustomData",
	    "Given node is not listed in its own partlist!");
	return;
    }
#endif

    // O.K., we're going to do something a little tricky here.  You see, some
    // of the constructors for nodekits build parts automatically. For example,
    // an SoV1ConeKit constructor creates a new SoCone and then calls
    // setPart( "cone", newlyMadeSoCone );
    // Now, reading in a nodekit follows the following algorithm:
    // [1] create a new kit
    // [2] read the kit's fields and children
    // [3] come into this routine and check the node's structural integrity,
    //     based on the information you find in the field 'customData'
    // The problem is that steps [1] and [2] can create duplicate children.
    // Clearly, we want to keep the children described in the file, which 
    // are created/read during step [2] and enumerated in the 'customData' 
    // field.
    // Hence, the tricky part:
    //   If 'customData' is empty, then keep the default parts made by
    //   the constructor.
    //   If 'customData' is not empty, then we want to get rid of the 
    //   parts made in the constructor. These parts will all be registered
    //   in the partsList.  So get rid of everything in the partsList (except
    //   for 'this' of course).
    if ( !inst->customData.isDefault() && 
	 (inst->customData[(int) index] != "") ) {
	SbName entryName;
	for ( int entryNum  = nkp->numEntries - 1; entryNum > 0; entryNum-- ) {
	    // Work backwards through the parts list. Then we will be
	    // sure to delete children before their parents.
	    // Also, don't get rid of entry 0, since that is 'this'.
	    if ( nkp->nodeList[entryNum] != NULL ) {
		entryName = theCat->getName( entryNum );
		nkp->setAnyPart(entryName, NULL, TRUE );
	    }
	}
    }
    // end of tricky part.

    // Get a destroyable copy of the customData string.(strtok will corrupt it).
    char *fData = strdup( inst->customData[(int) index].getString());
    // to begin, advance fData to after the HEADER
    fData += strlen( SO_V1_BASEKIT_FILEDATA_HEADER );

    char    *newPartName, *newChildNumString;
    int     newChildNum, partNum, parentPartNum;
    SoGroup *parentNode;
    SoNode  *childNode;
    SbBool  isOk;

    newPartName       = strtok( fData, " \n");  // first string
    newChildNumString = strtok( NULL, " \n");   // second string

    // as long as we can keep reading in name/number pairs, continue...
    while ( newPartName && newChildNumString ) {

	isOk = TRUE;

	// for each partName/childNumber pair, find the part,
	// find that part's parent, and set the properly numbered child 
	// of that parent to be 'partName'
	sscanf( newChildNumString, "%d", &newChildNum );

	// find and check the partNum for this part	
	partNum = theCat->getPartNumber( newPartName );
	if ( partNum == SO_V1_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	    SoDebugError::postWarning("SoV1BaseKit::interpretCustomData",
		"node type %s wont recognize the part named %s",
		    inst->getTypeId().getName().getString(), newPartName );
#endif
	    isOk = FALSE;
	}
	
	// find and check the parentPartNum
	if ( isOk ) { 
	    parentPartNum = theCat->getParentPartNumber( newPartName );
	    if ( parentPartNum == SO_V1_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
		SoDebugError::postWarning("SoV1BaseKit::interpretCustomData",
		    "node type %s has no parent part for part named %s", 
			inst->getTypeId().getName().getString(), newPartName );
#endif
		isOk = FALSE;
	    }
	}

	// make sure parent is already in the parts list!
	if ( isOk ) {
	    if (nkp->nodeList[parentPartNum] == NULL) {
#ifdef DEBUG
		SoDebugError::post("SoV1BaseKit::interpretCustomData",
		    "parent of node %s is not an existing node yet.",
					newPartName );
#endif
		isOk = FALSE;
	    }
	}

	// make sure parent has enough children to get the one specified
	if ( isOk ) {
	    parentNode = (SoGroup *) nkp->nodeList[parentPartNum];
	    if ( !parentNode ||
		newChildNum >= parentNode->getNumChildren() ) {
#ifdef DEBUG
		SoDebugError::post("SoV1BaseKit::interpretCustomData",
		    "parent of node %s only has %d children, but you are asking for child number %d", newPartName, parentNode->getNumChildren(), newChildNum );
#endif
		isOk = FALSE;
	    }
	}

	// make sure child is of type given by the template
	if ( isOk ) {
	    childNode = parentNode->getChild( newChildNum );
	    if ( !childNode || 
		 !childNode->isOfType( theCat->getType( partNum ) ) ) {
#ifdef DEBUG
		SoDebugError::post("SoV1BaseKit::interpretCustomData",
		    "given part not of correct type");
#endif
		isOk = FALSE;
	    }
	}

	// make sure the spot isnt already taken...
	if ( isOk ) {
	    if ( nkp->nodeList[partNum] != NULL ) {
#ifdef DEBUG
		SoDebugError::post("SoV1BaseKit::interpretCustomData",
		    " given part %s already in parts list", newPartName );
#endif
		isOk = FALSE;
	    }
	}

	if ( isOk && theCat->isList( partNum ) ) {
	    const SbPList listTypes = theCat->getListItemTypes( partNum );
	    SoType  *babyType;
	    SoNode  *theBaby;
	    SoGroup *grp = (SoGroup *) childNode;

	    for (int babyNum = 0;
		 (babyNum < grp->getNumChildren()) && (isOk == TRUE);
		 babyNum++ ) {

		theBaby = grp->getChild( babyNum );
		SbBool  foundBaby = FALSE;

		for ( int j = 0;
		      (j < listTypes.getLength()) && (foundBaby == FALSE); j++ ) {
		    babyType = (SoType *) listTypes[j];

		    if ( theBaby->isOfType( *babyType ) )
			foundBaby = TRUE;
		}
		if ( foundBaby == FALSE ) {
#ifdef DEBUG
		    SoDebugError::post("SoV1BaseKit::interpretCustomData",
			" part %s is list, but children chosen node are not of the list item type", newPartName );
#endif
		    isOk = FALSE;
		}
	    }
	}

	// passed every test!  set that part...
	if ( isOk ) {
	    nkp->nodeList[ partNum ] = childNode;
	}

	newPartName       = strtok( NULL, " \n");  // first string
	newChildNumString = strtok( NULL, " \n");   // second string
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create the V2.0 node to replace this node
//
// Use: public

SoNode *
SoV1BaseKit::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoBaseKit;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method to setUp the new node.  
//
// Use: public

void
SoV1BaseKit::setUpNewNode(SoNode *newNode)
//
////////////////////////////////////////////////////////////////////////
{
    const SoV1NodekitCatalog *cat = getNodekitCatalog();

    SoBaseKit *newKit = (SoBaseKit *) newNode;

    // We're going to be undoing some parent child connecions.
    // So, just to be safe, ref every non-NULL node in the parts list,
    // and unref them at the end...
    int i;
    for( i = 1; i < cat->getNumEntries(); i++ ) {
	SoNode *thePart = nodekitPartsList->nodeList[i];
	if (thePart != NULL)
	    thePart->ref();
    }

    // Loop through all parts in the parts list.
    // Start at 1, since part 0 in the list is 'this'
    for( i = 1; i < cat->getNumEntries(); i++ ) {

	SoNode *thePart = nodekitPartsList->nodeList[i];
	const SbName &theName = cat->getName( i );

	// If the part is not a leaf part, (and also is non-NULL)
	// then remove any children it may have. 
	// Why? Well, at this point, we've got all the node pointers 
	// established in the parts list, so we won't lose track of them.
	// When we call set part in the new node, it will establish the
	// correct locations for each part in the new kit. But we don't
	// want to keep the old parent-child relationships that we read from 
	// file.
	if ( thePart != NULL && !cat->isLeaf(i)) {
	    if (thePart->isOfType( SoGroup::getClassTypeId() )) {
		SoGroup *grp = (SoGroup *) thePart;
		while ( grp->getNumChildren() > 0 ) {
		    grp->removeChild(0);
		}
	    }
	}


	// Attempt to just call setPart with the entry in the list...
	if ( !tryToSetPartInNewNode( newKit, thePart, theName ) ) {
	    
	    // Okay, we couldn't just set the part.
	    // Try to do some of the special case stuff.
	    if ( !dealWithUpgradedPart( newKit, thePart, theName )) {
#ifdef DEBUG
		SoDebugError::post("SoV1BaseKit::setUpNewNode",
		    "Dont know what to do with part named %s. The node is going to be ignored and lost.", theName.getString() );
#endif
	    }
	}
    }

    // Unref the parts we ref'ed
    for( i = 1; i < cat->getNumEntries(); i++ ) {
	SoNode *thePart = nodekitPartsList->nodeList[i];
	if (thePart != NULL)
	    thePart->unref();
    }

}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Attempt to set the given part in the new node.
//    This will succeed if the partName exists in the newNode's catalog and
//    the node that we are attempting to use for that part is of a legal type.
//
// Use: protected
//
SbBool 
SoV1BaseKit::tryToSetPartInNewNode( SoBaseKit *newNode,
				    SoNode *newPart, const SbName &newPartName )
//
////////////////////////////////////////////////////////////////////////
{
    const SoNodekitCatalog *cat = newNode->getNodekitCatalog();

    // Does newNode's catalog contain an entry called 'newPartName'?
    int partNumber = cat->getPartNumber( newPartName );
    if ( partNumber == SO_CATALOG_NAME_NOT_FOUND ) {
	if ( newPart != NULL )
	    return FALSE;
	else
	    // Everythings okay, really, if the part we were going to give it
	    // was just NULL anyway.
	    return TRUE;
    }

    // Is newPart either NULL or derived from the class given in newNode's
    // catalog for 'newPartName'
    const SoType &partType = cat->getType( partNumber );
    if ( newPart != NULL && newPart->isOfType( partType ) == FALSE )
	return FALSE;

    // If we got here, we can call setAnyPart!
    newNode->setAnyPart( newPartName, newPart );
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    A virtual function that allows classes to treat parts that fail the
//    tests of 'tryToSetPartInNewNode'
//
//    For SoV1BaseKit, this means dealing with the part 'label' and
//    dealing with any part that is a list part.
//
//    Subclasses will have to do special work if, for example, a part name
//    or part type has been changed  or if a part has been removed.
//
// Use: public
//
SbBool 
SoV1BaseKit::dealWithUpgradedPart( SoBaseKit *newNode, SoNode *newPart, 
				    const SbName &newPartName )
//
////////////////////////////////////////////////////////////////////////
{
    // If the part name is "label", let's just name the newKit
    // "label", since the label part no longer exists and most likely
    // this is the appropriate functionality.
    if ( newPartName == "label" ) {
	if ( !newPart->isOfType( SoLabel::getClassTypeId() ) )
	    return FALSE;

	SoLabel *lbl = (SoLabel *) newPart;
	newNode->setName(lbl->label.getValue());
	return TRUE;
    }

    // If the part doesn't exist in the catalog, there is nothing more 
    // we can do just here.
    const SoNodekitCatalog *cat = newNode->getNodekitCatalog();
    int partNumber = cat->getPartNumber( newPartName );
    if ( partNumber == SO_CATALOG_NAME_NOT_FOUND )
	return FALSE;

    // If the part was a list part, we've got more work to do...
    if ( cat->isList( partNumber ) ) {
	// Old lists used to be just some subclass of group with children
	// stuck underneath.
	// New lists are of type SoNodeKitListPart. Within this part is
	// a container node, and below it are children of a given type.

	// First, get a list from the new node. 
	SoNodeKitListPart *newList 
	    = (SoNodeKitListPart *) newNode->getAnyPart( newPartName, TRUE);

	// For each child of the old list, try to set it as a child
	// in the new list.
#ifdef DEBUG
	if ( !newPart->isOfType( SoGroup::getClassTypeId() ) )  {
	    SoDebugError::post("SoV1BaseKit::dealWithUpgradedPart",
		"the node for part %s in the old node isn't subclassed from SoGroup", newPartName.getString() );
		return FALSE;
	}
#endif
	// Check if all the children of oldList are legal children for newList
	SoGroup *oldList = (SoGroup *) newPart;
	SbBool allOkay = TRUE;
	int i;
	for ( i = 0; i < oldList->getNumChildren(); i++ ) {
	    SoNode *kid = oldList->getChild(i);
	    if ( newList->isChildPermitted( kid ) == FALSE )
		allOkay = FALSE;
	}

	// If the children aren't ALL okay, then don't set any. Just return
	// FALSE and hope that a subclass will be able to deal with them.
	if (allOkay == FALSE)
	    return FALSE;

	// Move all the children from oldList to newList...
	for ( i = 0; i < oldList->getNumChildren(); i++ ) {
	    SoNode *kid = oldList->getChild(i);
	    newList->addChild( kid );
	}
	while( oldList->getNumChildren() > 0 )
	    oldList->removeChild(0);

	// If the listContainer type is an SoSwitch, then copy the
	// whichChild value into the new list.
	if ( oldList->isOfType( SoSwitch::getClassTypeId() )
	    && newList->getContainerType().isDerivedFrom( 
		    SoSwitch::getClassTypeId())) {
	    SoSwitch *sw = (SoSwitch *) oldList;
	    // Send a string that says "whichChild x" to newList...
	    char myBuf[200];
	    sprintf( myBuf, "whichChild %d", sw->whichChild.getValue() );
	    newList->containerSet( myBuf );
	}
	return TRUE;
    }

    return FALSE;   // don't know how to do anything yet...
}
