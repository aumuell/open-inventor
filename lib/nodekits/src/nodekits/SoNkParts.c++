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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      SoNodekitParts
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodekits/SoNodekitParts.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/SoPath.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoGroup.h>
#include <malloc.h>

#include <SoDebug.h>

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoNodekitParts
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoNodekitParts::SoNodekitParts( SoBaseKit *rootOfKit )
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if ( rootOfKit == NULL ) {
	SoDebugError::post("SoNodekitParts::SoNodekitParts",
    "rootOfKit is NULL. Can not continue construction. Expect a core dump");
	return;
    }
#endif

    rootPointer = rootOfKit;
    catalog = rootOfKit->getNodekitCatalog();  // assign the catalog

    numEntries = catalog->getNumEntries();        // make a empty node list
    fieldList = new ( SoSFNode *[numEntries] );

    // Make each field in the fieldList point to the field corresponding
    // to the similarly indexed catalog entry.
    SbName partName;
    SbName fieldName;

    for ( int i = 0; i < numEntries; i++ ) {
	// Get the name of the part from the catalog
	 partName = catalog->getName( i );

	// Find the field for that part, and set fieldList entry 
	// to point at it.
	 if ( i == SO_CATALOG_THIS_PART_NUM ) {
	    // For the part "this" we do NOT fill in a field. We set it to
	    // NULL and are careful not to try to look at its contents.
	    fieldList[i] = NULL;
	 }
	 else {
	    const SoFieldData *fData;
	    fData = rootOfKit->getFieldData();
	    for ( int j = 0; j < fData->getNumFields(); j++ ) {
		fieldName = fData->getFieldName(j);
		if (partName == fieldName) {
		    SoSFNode *theField =
		    		(SoSFNode *) fData->getField(rootOfKit,j);
		    fieldList[i] = theField;

		    // Turn off notification on this field.
		    // We store the info as a field, but unless we 
		    // turn off notification, everything takes forever.
		    theField->enableNotify(FALSE);
		}
	    }
	 }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: internal

SoNodekitParts::~SoNodekitParts()
//
////////////////////////////////////////////////////////////////////////
{
    // delete the nodelist
    delete [ /*numEntries*/ ] fieldList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates the node for the part asked for.
//    If the part already exists, then just return TRUE without doing anything
//    If not, it creates the part and all necessary parts up to the top of
//    of the nodekit, and places them properly as children in the graph.
//
//    Note:
//       this routine will NOT search for the requested part name within the
//       catalogs of its child parts.
//
// Use: internal

SbBool
SoNodekitParts::makePart( const int partNum )
//
////////////////////////////////////////////////////////////////////////
{
    if ( partNum == SO_CATALOG_THIS_PART_NUM )
	return TRUE;
#ifdef DEBUG
    if ( !partFoundCheck( partNum ) )
	return FALSE;
#endif

    // if the part is already in the Node list, just return TRUE
    if ( verifyPartExistence( partNum ) )
	return TRUE;

    // create the node
    SoNode *inst = (SoNode *) 
		catalog->getDefaultType( partNum ).createInstance();
#ifdef DEBUG
    if ( inst == NULL ) {
	SoDebugError::post("SoNodekitParts::makePart",
    "Can't make part %s. It belongs to an abstract class. Bad parts catalog",
			    catalog->getName(partNum).getString());
	return FALSE;
    }
#endif

    // If it's a list part, set the containerType and childTypes...
    if ( catalog->isList( partNum ) ) {
	SoNodeKitListPart *lp = (SoNodeKitListPart *) inst;
	lp->setContainerType( catalog->getListContainerType( partNum ) );

	const SoTypeList childTypes = catalog->getListItemTypes( partNum );
	for ( int i = 0; i < childTypes.getLength(); i++ )
	    lp->addChildType( childTypes[i] );

	lp->lockTypes();
    }

    inst->ref(); // temporarily ref it, until it has a parent

    // create its parent part
    int parentPartNum = catalog->getParentPartNumber( partNum );
    // Turn off notification while building the parent part.
    // We'll be notifying when adding the lower part, so let's not
    // notify twice.
    SbBool wasEn = rootPointer->enableNotify(FALSE);
    SbBool madeOk = makePart( parentPartNum );
    rootPointer->enableNotify(wasEn);
    if ( ! madeOk ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitParts::makePart",
	    		   "Can\'t make parent for part named %s", 
			    catalog->getName(partNum).getString() );
#endif
	inst->unref();             // undo the temporary ref
	return FALSE;
    }

    // Don't need to check for 'this' part, since we would have returned
    // earlier from verifyPartExistence.
    fieldList[partNum]->setValue( inst );

    // find the next closest right sibling that already exists
    int    sibPartNum, searchPartNum;

    for( sibPartNum = -1, 
	 searchPartNum = catalog->getRightSiblingPartNumber( partNum );
	 sibPartNum == -1 && searchPartNum != SO_CATALOG_NAME_NOT_FOUND;
	 searchPartNum = catalog->getRightSiblingPartNumber(searchPartNum )) {
	if ( verifyPartExistence( searchPartNum ) )
	    sibPartNum = searchPartNum;
    }

    SoBaseKit *parentKit = NULL;
    SoGroup   *parentGroup = NULL;
    if ( parentPartNum == SO_CATALOG_THIS_PART_NUM )
	parentKit   = rootPointer;
    else
	parentGroup = (SoGroup *)   fieldList[parentPartNum]->getValue();

    if (sibPartNum == -1 ) {
	// no right sibling is made yet, so just add this as a child to parent
	if ( parentKit != NULL )
	    parentKit->addChild( inst );
	else
	    parentGroup->addChild( inst );
    }
    else {
	// a right sibling exists. Put this node just in front of it.
	if ( parentKit != NULL ) {
	int sibIndex = parentKit->findChild(fieldList[sibPartNum]->getValue());
	parentKit->insertChild( fieldList[partNum]->getValue(), sibIndex );
	}
	else {
	int sibIndex =parentGroup->findChild(fieldList[sibPartNum]->getValue());
	parentGroup->insertChild( fieldList[partNum]->getValue(), sibIndex );
	}
    }
    inst->unref();  // undo the temporary ref
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces the node described with the one passed in.
//
//    If the part already exists, then the current part is removed from 
//    its parent and the node list.
//
//    Next, the new part is put in the list and made the child of its parent.
//
//    Note:
//       this routine will NOT search for the requested part name within the
//       catalogs of its child parts.
//
// Use: internal

SbBool
SoNodekitParts::replacePart( const int partNum, SoNode *newPartNode )
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if ( partNum == SO_CATALOG_THIS_PART_NUM )
	return FALSE;
    if ( !partFoundCheck( partNum ) )
	return FALSE;
#endif

    // make sure the node given is of the proper type
    if ( newPartNode != NULL
	 && !newPartNode->isOfType( catalog->getType( partNum ) ) ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::replacePart",
			    "the given part is not of the correct type");
#endif
	return FALSE;
    }

    // If it's a list part, make sure all the types match okay.
    if ( newPartNode != NULL && catalog->isList( partNum ) ) {

	const SoTypeList  kidTypes = catalog->getListItemTypes( partNum );
	SoNodeKitListPart *lp = (SoNodeKitListPart *) newPartNode;

	// If types aren't locked, make this part have the right types,
	// then lock them.
	if ( !lp->isTypeLocked() ) {
	    lp->setContainerType(catalog->getListContainerType(partNum));
	    for (int j = 0; j < kidTypes.getLength(); j++)
		lp->addChildType( kidTypes[j] );
	    lp->lockTypes();
	}

	// Make sure container is of correct type.
	if ( lp->getContainerType() != catalog->getListContainerType(partNum)) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::replacePart",
			    "This new list part has wrong type container");
#endif
	    return FALSE;
	}

	SbBool okay = TRUE;

	// Make sure child types are all okay.
	for ( int i = 0; i < kidTypes.getLength(); i++ )
	    if (    lp->isTypePermitted( kidTypes[i]) == FALSE )
		okay = FALSE;

	if ( okay == FALSE ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::replacePart",
	"Allowable child types in given list do not match the specs in the catalog");
#endif
	    return FALSE;
	}
    }

    int parentPartNum = catalog->getParentPartNumber( partNum );

    // So they don't try replacing 'this'
    if ( !partFoundCheck( parentPartNum ) )
	return FALSE;

    // If new node isn't NULL, create its parent part
    if ( newPartNode != NULL ) {
	// Turn off notification while building the parent part.
	// We'll be notifying when adding the lower part, so let's not
	// notify twice.
	SbBool wasEn = rootPointer->enableNotify(FALSE);
	SbBool madeOk = makePart( parentPartNum );
	rootPointer->enableNotify(wasEn);
	if ( ! madeOk ) {
#ifdef DEBUG
	    SoDebugError::post( "SoNodekitParts::replacePart",
				"can't make parent for part named %s", 
				catalog->getName(partNum).getString() );
#endif
	    return FALSE;
	}
    }

    // Now we know there's a parent part, if we need one.
    // Set the appropriate parent pointers. 
	SoBaseKit *parentKit = NULL;
	SoGroup   *parentGroup = NULL;
	if ( parentPartNum == SO_CATALOG_THIS_PART_NUM )
	    parentKit   = rootPointer;
	else
	    parentGroup = (SoGroup *)   fieldList[parentPartNum]->getValue();

    // If an old part is already in the Node list, then
    // get rid of any parts that are its descendants.
    // This will not get rid of the old part itself.
	SoNode *oldPart = NULL;
	if ( verifyPartExistence( partNum ) ) {
	    // unregister all the descendant parts of this part by setting 
	    // the appropriate SoSFField values in the field list to NULL. 
	    unregisterDescendants( partNum );

	    oldPart = fieldList[partNum]->getValue();
	}

    // set the new part as the new field value.
        fieldList[partNum]->setValue( newPartNode);

    // If the new part is NULL, remove the oldPart from its parent.
    // Then, remove parent if it is no longer needed.
	if (newPartNode == NULL) {
	    if ( oldPart != NULL) {
		if (parentKit != NULL) 
		    parentKit->removeChild( oldPart );
		else if (parentGroup != NULL) {
		    // If the parent is a switch, make sure this doesn't 
		    // screw it up...
		    parentGroup->removeChild( oldPart );
		    if ( parentGroup->isOfType( SoSwitch::getClassTypeId() ) ){
			SoSwitch *sw = (SoSwitch *) parentGroup;
			int swNum = sw->getNumChildren();
			if (sw->whichChild.getValue() >= swNum)
			    sw->whichChild.setValue(  swNum - 1 );
		    }
		}
	    }
	    // We just set partNum to NULL and removed from its parent.
	    // Get rid of the parent, too, if we can...
	    if ( partIsNonLeafAndMayBeDeleted( parentPartNum ) )
		replacePart( parentPartNum, NULL );

	    return TRUE;
	}

    // If the new part is not a leaf in the structure, it had better
    // not have any children.  You've got to set the children after setting
    // the internal part.
        if ( catalog->isLeaf( partNum ) == FALSE ) {
	    SoGroup *partAsGroup = (SoGroup *) newPartNode;
	    if ( partAsGroup->getNumChildren() != 0 ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::replacePart",
		"The given part is a non-leaf in the catalog and contains children. When setting a non-leaf, the node must have 0 children.");
#endif
		return FALSE;
	    }
	}

    // If there's already a part, then we'll use replace child.
    // This way, the new part will be installed in any paths containing oldPart
    // Then return.
	if (oldPart != NULL) {
	    if ( parentKit != NULL )
		parentKit->replaceChild( oldPart, newPartNode );
	    else if (parentGroup != NULL)
		parentGroup->replaceChild( oldPart, newPartNode );

	    return TRUE;
	}

    // There was no old part.
    // Find the next closest right sibling that already exists
	int sibPartNum, searchPartNum;
	for( sibPartNum = -1, 
	     searchPartNum = catalog->getRightSiblingPartNumber( partNum );
	     sibPartNum == -1 && searchPartNum != SO_CATALOG_NAME_NOT_FOUND;
	     searchPartNum = catalog->getRightSiblingPartNumber(searchPartNum)){
	    if ( verifyPartExistence( searchPartNum ) )
		sibPartNum = searchPartNum;
	}

    // If newPartNode is going to be the rightmost child, use addChild...
	if (sibPartNum == -1 ) {
	    if ( parentKit != NULL )
		parentKit->addChild( newPartNode );
	    else if ( parentGroup != NULL )
		parentGroup->addChild( newPartNode );
	    return TRUE;
	}

    // Otherwise, insert the new part in front of the sibling.
	SoNode *sibNode = fieldList[sibPartNum]->getValue();
	if ( parentKit != NULL ) {
	    int sibIndex = parentKit->findChild( sibNode );
	    parentKit->insertChild( newPartNode, sibIndex );
	}
	else {
	    int sibIndex =parentGroup->findChild( sibNode );
	    parentGroup->insertChild( newPartNode, sibIndex );
	}

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called when 'partNum' is being set to a new value using replacePart.
//
//    Makes sure that all descendant parts of 'partNum' get NULLed out,
//    and is called prior to installing the new node value for 
//    'partNum'.
//
//    This is a dangerous and special purpose routine.
//
// Use: internal

void
SoNodekitParts::unregisterDescendants( const int partNum )
//
////////////////////////////////////////////////////////////////////////
{
    // For each part in the catalog...
    numEntries = catalog->getNumEntries();
    for ( int i = 0; i < numEntries; i++ ) {
	// If it's parent is partNum...
	if ( catalog->getParentPartNumber(i) == partNum ) {
	    unregisterDescendants( i );
	    // Don't need to worry about i being the index of 'this'.
	    // since 'this' has no parent part number to match.
	    fieldList[ i ]->setValue( NULL );
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  the part asked for
//    Note:
//       this routine will NOT search for the requested part name within the
//       catalogs of its child parts.
//
// Use: private

SoNode *
SoNodekitParts::getPartFromThisCatalog( const int partNum, 
				  SbBool makeIfNeeded, SbBool leafCheck, 
				  SbBool publicCheck, SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    existedBefore = FALSE;

    if ( !partFoundCheck( partNum ) )
	return FALSE;

    if ( partNum == SO_CATALOG_THIS_PART_NUM  ||
	 fieldList[partNum]->getValue() != NULL )
        existedBefore = TRUE;

    if ( leafCheck ) {
	if ( !partIsLeafCheck( partNum ) )
	    return FALSE;
    }
    if ( publicCheck ) {
	if ( !partIsPublicCheck( partNum ) )
	    return FALSE;
    }

    if ( partNum == SO_CATALOG_THIS_PART_NUM )
	return rootPointer;

    // Return if part already exists...
    if ( verifyPartExistence(partNum) )
	return ( fieldList[ partNum ]->getValue() );

    // Part doesn't exist yet.  Either return...
    if ( makeIfNeeded == FALSE )
	return NULL;

    // or make the part
    if ( makePart( partNum ) == TRUE )
	return ( fieldList[ partNum ]->getValue() );   // it was made O.K.
    else
	return NULL;                      // it didn't get made properly
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  path to the part asked for
//    Note:
//       this routine will NOT search for the requested part name within the
//       catalogs of its child parts.
//
// Use: private

SoFullPath *
SoNodekitParts::createPathToPartFromThisCatalog( const int partNum, 
		    SbBool makeIfNeeded, SbBool leafCheck, 
		    SbBool publicCheck,  SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *n = getPartFromThisCatalog( partNum, makeIfNeeded, 
					leafCheck, publicCheck, existedBefore );

    return( createPathDownTo( catalog->getName(partNum), n ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets  the part given by nameOfPart
//    Note:
//       this routine will NOT search for the requested part name within the
//       catalogs of its child parts.
//
// Use: private

SbBool
SoNodekitParts::setPartFromThisCatalog( const int partNum, 
				         SoNode *newPartNode, SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if ( partNum == SO_CATALOG_THIS_PART_NUM ) {
	SoDebugError::post("SoNodekitParts::setPartFromThisCatalog",
	       "You can not never set the part \"this\" after construction");
	return FALSE;
    }
#endif

    if ( !partFoundCheck( partNum ) )
	return FALSE;
    if ( !anyPart ) {
	if ( !partIsLeafCheck( partNum ) )
	    return FALSE;
	if ( !partIsPublicCheck( partNum ) )
	    return FALSE;
    }

    // otherwise, we need to replace the part that is currently being used
    return( replacePart( partNum, newPartNode ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  the part asked for
//    Assumes that 'nameOfPart' does not need to be parsed into
//    names separated by '.' 
//
// Use: public

SoNode *
SoNodekitParts::getSingleNamePart( const SbName &nameOfPart, 
		    SbBool makeIfNeeded, SbBool leafCheck, SbBool publicCheck,
		    SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    existedBefore = FALSE;

    // IS THERE A BRACKET, WHICH SIGNIFIES INDEXING INTO A LIST?
    if ( strrchr( nameOfPart.getString(), '[') != NULL ) {

	char *listNameCopy = strdup( nameOfPart.getString());
	int  arrayIndex;

	if ( !parseListItem( listNameCopy, arrayIndex)) {
	    free ( listNameCopy );
	    return NULL;
	}

	// get the list given by 'listNameCopy'
	SbBool listExistedBefore;
	SoNode *n = getSingleNamePart( listNameCopy, makeIfNeeded, TRUE, 
					publicCheck, listExistedBefore );
	if ( n == NULL ) {
	    free ( listNameCopy );
	    return NULL;
	}

	if ( !n->isOfType( SoNodeKitListPart::getClassTypeId() ) ){
#ifdef DEBUG
	    SoDebugError::post("SoNodekitParts::getSingleNamePart",
	    		       "part specified %s was not a list", listNameCopy );
#endif
	    if ( listExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( listNameCopy, NULL, TRUE );
	    }
	    free( listNameCopy);
	    return NULL;
	}

	SoNodeKitListPart *listGroup = (SoNodeKitListPart *) n;

	// If they are asking for the next child in the list, 
	// try to make it for them.  return this new node if successful.
	if ((listGroup->getNumChildren() == arrayIndex) && makeIfNeeded ) {
	    SoNode *newNode = NULL;
	    SoTypeList itemTypesList = listGroup->getChildTypes();
	    for ( int i = 0; i < itemTypesList.getLength(); i++ ) {

		// Can this child type create an instance?
		// (and is it a type of node?)

		if ( itemTypesList[i].canCreateInstance() && 
		     itemTypesList[i].isDerivedFrom( SoNode::getClassTypeId())){

		    // Create an instance, and add it to the listGroup...
		    newNode = (SoNode *) itemTypesList[i].createInstance();
		    listGroup->addChild( newNode );
		    break;
		}
	    }
	    if (newNode == NULL ) {
#ifdef DEBUG
		if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
		    fprintf(stderr,"\n");
		    fprintf(stderr,"NODE KIT PARTS DEBUG:\n");
		    fprintf(stderr,"    The part %s does not exist\n",
					nameOfPart.getString());
		    fprintf(stderr,"    and could not be created.The list %s\n",
					    listNameCopy );
		    fprintf(stderr,"    only specifies abstract types\n");
		    fprintf(stderr,"    Returning NULL\n");
		    fprintf(stderr,"\n");
		}
#endif
	    }
	    free( listNameCopy);
	    return( newNode );
	}

	// Return if child doesn't exist yet, and we were not able to
	// create one for them.
	if ( listGroup->getNumChildren() <= arrayIndex ) {
	    if ( makeIfNeeded ) {
#ifdef DEBUG
		if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
		    fprintf(stderr,"\n");
		    fprintf(stderr,"NODE KIT PARTS DEBUG:\n");
		    fprintf(stderr,"    Could not return the part\n");
		    fprintf(stderr,"    %s. The list %s only has\n", 
					nameOfPart.getString(), listNameCopy );
		    fprintf(stderr,"    %d entries. You want entry number %d\n",
					listGroup->getNumChildren(),arrayIndex);
		    fprintf(stderr,"    Returning NULL\n");
		    fprintf(stderr,"\n");
		}
#endif
	    }
	    if ( listExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( listNameCopy, NULL, TRUE );
	    }
	    free( listNameCopy);
	    return NULL;
	}

	// return entry number 'arrayIndex'
	existedBefore = TRUE;
	free( listNameCopy);
	return( listGroup->getChild( arrayIndex ) );
    }

    // NOT A LIST ITEM, IF WE GOT HERE.

    // IS THE REQUESTED PART IN THIS CATALOG?
    int partNum = catalog->getPartNumber( nameOfPart );
    if ( partNum != SO_CATALOG_NAME_NOT_FOUND )

	// IF SO, THEN GET IT FROM THIS CATALOG
	return ( getPartFromThisCatalog( partNum, makeIfNeeded,leafCheck, 
					    publicCheck, existedBefore ));
    else {

	// ELSE, SEARCH THE CATALOG RECURSIVELY FOR THE DESIRED PART
	// we need to pass a list to the recursive search saying which
	// types of nodes we have already checked.  This avoids infinite
	// loop (does chicken contain egg? does egg contain chicken? etc...)
	SoTypeList *typesChecked = new SoTypeList();
	typesChecked->append( catalog->getType(SO_CATALOG_THIS_PART_NUM) );

	for (int i = 0; i < numEntries; i++ ) {

	    // does it lie within this 'intermediary' part?
	    if ( catalog->recursiveSearch(i,nameOfPart,typesChecked) == TRUE) {

		delete typesChecked;  // don't need this anymore

		// if not making parts and 'intermediary' is NULL...
		if ( makeIfNeeded == FALSE && !verifyPartExistence( i ) )
		    return NULL;

		SbBool kitExistedBefore = (fieldList[i]->getValue() != NULL);

		// create the intermediary part...
		// Turn off notification while building the parent part.
		// We'll be notifying when adding the lower part, so let's not
		// notify twice.
		SbBool wasEn = rootPointer->enableNotify(FALSE);
		SbBool madeOk = makePart( i );
		rootPointer->enableNotify(wasEn);
		if ( ! madeOk )
		    return NULL;   // error making the part
		// NOTE: it's okay to cast the node into a base kit here,
		// since the recursive search would only have worked
		// if it was a nodekit
		// UGLY, yes, but it works.
#ifdef DEBUG
		// supposedly unnecessary type checking:
		if ( !fieldList[i]->getValue()->isOfType( 
					SoBaseKit::getClassTypeId() ) )
		    return NULL;
#endif
		SoBaseKit *intermediary =(SoBaseKit *)fieldList[i]->getValue(); 

		// Now that intermediary is built, get the part from within it
		// Call the method on the parts list, not the node.
		// It's more efficient and won't do extra virtual things that
		// might be added by subclasses of SoBaseKit.
		SoNodekitParts *interParts = intermediary->nodekitPartsList;
		SbBool answerExistedBefore;
		SoNode *answer = interParts->getAnyPart(nameOfPart,makeIfNeeded,
				leafCheck, publicCheck, answerExistedBefore );
		if (answer == NULL && kitExistedBefore == FALSE ) {
		    // if we just created the kit, we'd better get rid of it...
		    replacePart( i, NULL );
		}
		existedBefore = kitExistedBefore && answerExistedBefore;
		return answer;
	    }
	}
	delete typesChecked;  // don't need this anymore

    }

    // IF YOU GOT HERE, PARTNAME WAS NOT FOUND IN THE CATALOG OR ANY
    // OF THE SUB CATALOGS.
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
	fprintf(stderr,"\n");
	fprintf(stderr,"NODE KIT PARTS DEBUG: The catalog for this class\n");
	fprintf(stderr,"    of nodekit does not have a part named %s, \n",
			    nameOfPart.getString() );
	fprintf(stderr,"    nor do the catalogs for any of nested nodekits\n");
	fprintf(stderr,"    Returning NULL\n");
	fprintf(stderr,"\n");
    }
#endif
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  a path to the part asked for
//    Assumes that 'nameOfPart' does not need to be parsed into
//    names separated by '.' 
//
// Use: public

SoFullPath *
SoNodekitParts::getSingleNamePathToPart(const SbName &nameOfPart, 
				SbBool makeIfNeeded, SbBool leafCheck, 
				SbBool publicCheck,  SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    existedBefore = FALSE;

    SoFullPath *answerPath = NULL;

    // IS THERE A BRACKET, WHICH SIGNIFIES INDEXING INTO A LIST?
    if ( strrchr( nameOfPart.getString(), '[') != NULL ) {

	char *listNameCopy = strdup( nameOfPart.getString());
	int  arrayIndex;

	if (!parseListItem( listNameCopy, arrayIndex)){
	    free(listNameCopy);
	    return NULL;
	}

	// get the list given by 'listNameCopy'
	SbBool listExistedBefore;
	answerPath = getSingleNamePathToPart( listNameCopy, makeIfNeeded, 
				      TRUE, publicCheck, listExistedBefore );
	if (answerPath == NULL){
	    free(listNameCopy);
	    return NULL;
	}

	answerPath->ref();

	if (!answerPath->getTail()->isOfType(
				    SoNodeKitListPart::getClassTypeId()))  {
#ifdef DEBUG
	      SoDebugError::post("SoNodekitParts::getSingleNamePathToPart",
	      "list named %s is not actually a list part\n", listNameCopy );
#endif
		answerPath->unref();
		if ( listExistedBefore == FALSE ) {
		    // if we just created the list, we'd better get rid of it...
		    setSingleNamePart( listNameCopy, NULL, TRUE );
		}
		free(listNameCopy);
		return NULL;
	}

	SoNodeKitListPart *listGroup 
	    = (SoNodeKitListPart *) answerPath->getTail();

	// If they are asking for the next child in the list, 
	// try to make it for them.  return this new node if successful.
	if ((listGroup->getNumChildren() == arrayIndex) && makeIfNeeded ) {
	    SoNode *newNode = NULL;
	    SoTypeList itemTypesList = listGroup->getChildTypes();
	    for ( int i = 0; i < itemTypesList.getLength(); i++ ) {

		// Can this child type create an instance?
		// (and is it a type of node?)

		if ( itemTypesList[i].canCreateInstance() && 
		     itemTypesList[i].isDerivedFrom( SoNode::getClassTypeId())){

		    // Create an instance, and add it to the listGroup...
		    newNode = (SoNode *) itemTypesList[i].createInstance();
		    listGroup->addChild( newNode );

		    // Now we extend the path to our new node.
		    // First, append '0' for the container node.
		    // Second, append the given index within the container node.
		    answerPath->append( 0 );
		    answerPath->append( arrayIndex );
		    answerPath->unrefNoDelete();
		    break;
		}
	    }
	    if (newNode == NULL ) {
#ifdef DEBUG
		if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
		    fprintf(stderr,"\n");
		    fprintf(stderr,"NODE KIT PARTS DEBUG:\n");
		    fprintf(stderr,"    The part %s does not exist\n",
					nameOfPart.getString());
		    fprintf(stderr,"    and could not be created.The list %s\n",
					    listNameCopy );
		    fprintf(stderr,"    only specifies abstract types\n");
		    fprintf(stderr,"    Returning NULL\n");
		    fprintf(stderr,"\n");
		}
#endif
	        answerPath->unref();
		answerPath = NULL;
	    }
	    free( listNameCopy);
	    return answerPath;
	}

	// Return if child doesn't exist yet, and we were not able to
	// create one for them.
	if ( listGroup->getNumChildren() <= arrayIndex ) {
	    if ( makeIfNeeded ) {
#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
	    fprintf(stderr,"\n");
	    fprintf(stderr,"NODE KIT PARTS DEBUG: Could not return the part\n");
	    fprintf(stderr,"    %s. The list %s only has\n", 
				nameOfPart.getString(), listNameCopy );
	    fprintf(stderr,"    %d entries, but you want entry number %d\n",
				listGroup->getNumChildren(), arrayIndex);
	    fprintf(stderr,"    Returning NULL\n");
	    fprintf(stderr,"\n");
	    }
#endif
	    }
	    answerPath->unref();
	    if ( listExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( listNameCopy, NULL, TRUE );
	    }
	    free(listNameCopy);
	    return NULL;
	}

	// we have path to list, so append to it:
	// First, append '0' for the container node.
	// Second, append the given index within the container node.
	answerPath->append( 0 );
	answerPath->append( arrayIndex );
	answerPath->unrefNoDelete();
	existedBefore = TRUE;
	free(listNameCopy);
	return answerPath;
    }

    // NOT A LIST ITEM, IF WE GOT HERE.

    // IS THE REQUESTED PART IN THIS CATALOG?
    int partNum = catalog->getPartNumber( nameOfPart );
    if ( partNum != SO_CATALOG_NAME_NOT_FOUND )

	// IF SO, THEN GET IT FROM THIS CATALOG
	return ( createPathToPartFromThisCatalog( partNum, makeIfNeeded,
				 leafCheck, publicCheck, existedBefore ) );

    else {

	// ELSE, SEARCH THE CATALOG RECURSIVELY FOR THE DESIRED PART
	// we need to pass a list to the recursive search saying which
	// types of nodes we have already checked.  This avoids infinite
	// loop (does chicken contain egg? does egg contain chicken? etc...)
	SoTypeList *typesChecked = new SoTypeList();
	typesChecked->append( catalog->getType(SO_CATALOG_THIS_PART_NUM) );

	for (int i = 0; i < numEntries; i++ ) {

	    // does it lie within this 'intermediary' part?
	    if ( catalog->recursiveSearch(i,nameOfPart,typesChecked) == TRUE){

		delete typesChecked;  // don't need this anymore

		// if not making parts and 'intermediary' is NULL...
		if ( makeIfNeeded == FALSE && !verifyPartExistence( i ) )
		    return NULL;

		SbBool kitExistedBefore = (fieldList[i]->getValue() != NULL);

		// create the intermediary part...
		const SbName interName = catalog->getName(i);
		// Turn off notification while building the parent part.
		// We'll be notifying when adding the lower part, so let's not
		// notify twice.
		SbBool wasEn = rootPointer->enableNotify(FALSE);
		SbBool madeOk = makePart( i );
		rootPointer->enableNotify(wasEn);
		if ( ! madeOk )
		    return NULL;   // error making the part
		// NOTE: it's okay to cast the node into a base kit here,
		// since the recursive search would only have worked
		// if it was a nodekit
		// UGLY, yes, but it works.
#ifdef DEBUG
		// supposedly unnecessary type checking:
		if ( !fieldList[i]->getValue()->isOfType( 
					SoBaseKit::getClassTypeId() ) )
		    return NULL;
#endif
		SoBaseKit *intermediary =(SoBaseKit *)fieldList[i]->getValue(); 

		// first, build a path from 'this' to 'intermediary'
		SoFullPath  *pathA = createPathDownTo( interName, intermediary);

		// next, build a path from 'intermediary' to the end...
		// Call the method on the parts list, not the node.
		// It's more efficient and won't do extra virtual things that
		// might be added by subclasses of SoBaseKit.
		SoNodekitParts *interParts = intermediary->nodekitPartsList;
		SbBool secondExistedBefore;
		SoFullPath *pathB = 
		    (SoFullPath *) interParts->createPathToAnyPart(nameOfPart,
				makeIfNeeded, leafCheck, 
				publicCheck, secondExistedBefore );
		if (pathA)
		    pathA->ref();
		if (pathB)
		    pathB->ref();

		if ( pathB == NULL && kitExistedBefore == FALSE ) {
		    // if we just created the kit, we'd better get rid of it...
		    replacePart( i, NULL );
		}
		existedBefore = kitExistedBefore && secondExistedBefore;

		// tack pathB onto pathA and return it
		SoFullPath   *totalPath = addPaths( pathA, pathB );

		if (pathA)
		    pathA->unref();
		if (pathB)
		    pathB->unref();

		return( totalPath );
	    }
	}
	delete typesChecked;  // don't need this anymore

    }

    // IF YOU GOT HERE, PARTNAME WAS NOT FOUND IN THE CATALOG OR ANY
    // OF THE SUB CATALOGS.
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
	fprintf(stderr,"\n");
	fprintf(stderr,"NODE KIT PARTS DEBUG: The catalog for this class\n");
	fprintf(stderr,"    of nodekit does not have a part named %s, \n",
			    nameOfPart.getString() );
	fprintf(stderr,"    nor do the catalogs for any of nested nodekits\n");
	fprintf(stderr,"    Returning NULL\n");
	fprintf(stderr,"\n");
    }
#endif
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets  the part given
//    Assumes that 'nameOfPart' does not need to be parsed into
//    names separated by '.' 
//
// Use: public

SbBool
SoNodekitParts::setSingleNamePart( const SbName &nameOfPart,SoNode *newPartNode,
				   SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    // IS THERE A BRACKET, WHICH SIGNIFIES INDEXING INTO A LIST?
    if ( strrchr( nameOfPart.getString(), '[') != NULL ) {
	char *listNameCopy = strdup( nameOfPart.getString());
	int  arrayIndex;

	if (!parseListItem( listNameCopy, arrayIndex )){
	    free(listNameCopy);
	    return FALSE;
	}

	// get the list given by 'listNameCopy'
	// Only need to create it if (newPartNode != NULL)
	// This is because, if new part is to be NULL, then the
	// list containing it need not exist either.
	SbBool needToMake = ( newPartNode != NULL);
	SbBool listExistedBefore;
	SoNode *n = getSingleNamePart( listNameCopy, needToMake, TRUE, 
				       !anyPart, listExistedBefore );
	if ( n == NULL && needToMake == FALSE ) {
	    free(listNameCopy);
	    return TRUE;
	}
	if ( n == NULL && needToMake == TRUE ) {
	    free(listNameCopy);
	    return FALSE;
	}

	if ( !n->isOfType( SoNodeKitListPart::getClassTypeId() ) ){
#ifdef DEBUG
	    SoDebugError::post("SoNodekitParts::setSingleNamePart",
	    "part specified %s was not a list", listNameCopy );
#endif
	    if ( listExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( listNameCopy, NULL, TRUE );
	    }
	    free(listNameCopy);
	    return FALSE;
	}

	SoNodeKitListPart *listGroup = (SoNodeKitListPart *) n;

	// handle case of setting to NULL
	if ( newPartNode == NULL ) {
	    // remove if you can...
	    if ( arrayIndex >= 0 && listGroup->getNumChildren() > arrayIndex )  
	         listGroup->removeChild( arrayIndex );
	    else if ( listExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( listNameCopy, NULL, TRUE );
	    }
	    free(listNameCopy);
	    return TRUE;
	}

	// check type...
	SoTypeList itemTypesList = listGroup->getChildTypes();
	for ( int i = 0; i < itemTypesList.getLength(); i++ ) {
	    if ( newPartNode->isOfType( itemTypesList[i] ) ) {
		// Set the node...
		if ( arrayIndex == listGroup->getNumChildren() )
		    // Create a new final entry, if a new index...
		    listGroup->insertChild(newPartNode, arrayIndex );
		else if ( arrayIndex < listGroup->getNumChildren() )
		    // Or, replace if it's not a new index...
		    listGroup->replaceChild(arrayIndex, newPartNode );
#ifdef DEBUG
		else if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
		    fprintf(stderr,"\n");
		    fprintf(stderr,"NODE KIT PARTS DEBUG:\n");
		    fprintf(stderr,"    The part %s could not be installed\n",
					nameOfPart.getString());
		    fprintf(stderr,"    because the index %d is greater \n",
					arrayIndex);
		    fprintf(stderr,"    than %d, the number of entries in \n",
					listGroup->getNumChildren());
		    fprintf(stderr,"    the list %s\n",
					    listNameCopy );
		    fprintf(stderr,"    Returning NULL\n");
		    fprintf(stderr,"\n");
		}
#endif
	        free(listNameCopy);
		return TRUE;
	    }
	}
	// child was not good for the list...
#ifdef DEBUG
	SoDebugError::post("SoNodekitParts::setSingleNamePart",
	"Node given is wrong type. Its type is %s, which is inappropriate for list named %s",
		 newPartNode->getTypeId().getName().getString(), listNameCopy );
#endif
	if ( listExistedBefore == FALSE ) {
	    // if we just created the list, we'd better get rid of it...
	    setSingleNamePart( listNameCopy, NULL, TRUE );
	}
	free(listNameCopy);
	return FALSE;
    }

    // NOT A LIST ITEM, IF WE GOT HERE.

    // IS THE REQUESTED PART IN THIS CATALOG?
    int partNum = catalog->getPartNumber( nameOfPart );
    if ( partNum != SO_CATALOG_NAME_NOT_FOUND )

	// IF SO, THEN SET IT USING THIS CATALOG
	return ( setPartFromThisCatalog( partNum, newPartNode, anyPart ) );

    else {

	// ELSE, SEARCH THE CATALOG RECURSIVELY FOR THE DESIRED PART
	// we need to pass a list to the recursive search saying which
	// types of nodes we have already checked.  This avoids infinite
	// loop (does chicken contain egg? does egg contain chicken? etc...)
	SoTypeList *typesChecked = new SoTypeList();
	typesChecked->append( catalog->getType(SO_CATALOG_THIS_PART_NUM));

	for (int i = 0; i < numEntries; i++ ) {

	    // does it lie within this 'intermediary' part?
	    if ( catalog->recursiveSearch(i,nameOfPart,typesChecked) == TRUE){

		delete typesChecked;  // don't need this anymore

		// If the part is not yet made, and the newPartNode is NULL,
		// we can just return. The part we seek to set to NULL
		// does not exist.
		if ( newPartNode == NULL   && !verifyPartExistence( i ) )
		    return TRUE;

		SbBool kitExistedBefore = (fieldList[i]->getValue() != NULL);

		// create the intermediary part...
		// Turn off notification while building the parent part.
		// We'll be notifying when adding the lower part, so let's not
		// notify twice.
		SbBool wasEn = rootPointer->enableNotify(FALSE);
		SbBool madeOk = makePart( i );
		rootPointer->enableNotify(wasEn);
		if ( ! madeOk )
		    return FALSE;   // error making the part
		// NOTE: it's okay to cast the node into a base kit here,
		// since the recursive search would only have worked
		// if it was a nodekit
		// UGLY, yes, but it works.
#ifdef DEBUG
		// supposedly unnecessary type checking:
		if ( !fieldList[i]->getValue()->isOfType( 
					    SoBaseKit::getClassTypeId() ) )
		    return FALSE;
#endif
		SoBaseKit *intermediary =(SoBaseKit *)fieldList[i]->getValue(); 

		// now that intermediary is built, set the part within it
		// Now that intermediary is built, set the part within it.
		// Call the method on the parts list, not the node.
		// It's more efficient and won't do extra virtual things that
		// might be added by subclasses of SoBaseKit.
		SoNodekitParts *interParts = intermediary->nodekitPartsList;
		SbBool didIt = interParts->setAnyPart(nameOfPart, newPartNode, 
						anyPart );
		if (didIt == FALSE && kitExistedBefore == FALSE ) {
		    // if we just created the kit, we'd better get rid of it...
		    replacePart( i, NULL );
		}
		return didIt;
	    }
	}
	delete typesChecked;  // don't need this anymore

    }

    // IF YOU GOT HERE, PARTNAME WAS NOT FOUND IN THE CATALOG OR ANY
    // OF THE SUB CATALOGS.
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_KIT_PARTS")) {
	fprintf(stderr,"\n");
	fprintf(stderr,"NODE KIT PARTS DEBUG: The catalog for this class\n");
	fprintf(stderr,"    of nodekit does not have a part named %s, \n",
			    nameOfPart.getString() );
	fprintf(stderr,"    nor do the catalogs for any of nested nodekits\n");
	fprintf(stderr,"    Returning NULL\n");
	fprintf(stderr,"\n");
    }
#endif
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  the part asked for
//
// Use: protected

SoNode *
SoNodekitParts::getAnyPart( const SbName &nameOfPart, SbBool makeIfNeeded, 
			    SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    SbBool existedBefore;
    return (getAnyPart( nameOfPart, makeIfNeeded, leafCheck, publicCheck,
			existedBefore ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  the part asked for
//
// Use: protected

SoNode *
SoNodekitParts::getAnyPart( const SbName &nameOfPart, SbBool makeIfNeeded, 
			    SbBool leafCheck, SbBool publicCheck,
			    SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    existedBefore = FALSE;

    char   *nameCopy, *firstName, *remainderString;
    SoNode *firstNode = NULL, *secondNode = NULL;

    // JUST A SINGLE NAME...
    if ( strrchr( nameOfPart.getString(), '.' ) == NULL )
	return getSingleNamePart( nameOfPart, makeIfNeeded, leafCheck, 
				  publicCheck, existedBefore );

    // OTHERWISE, A COMPOUND NAME of the form:  part1.part2.part3 etc.
    // copy the string
    nameCopy = strdup( nameOfPart.getString() );

    // break string into two parts
    firstName = strtok( nameCopy, ".");   // everything before the first '.'
    remainderString = strtok( NULL, "");  // everything else

    // get node for first part
    // intermediate names have to be leaves, so arg3 (leafCheck) is TRUE
    SbBool firstExistedBefore;
    firstNode = getSingleNamePart( firstName, makeIfNeeded, TRUE, 
				   publicCheck, firstExistedBefore );

    if ( firstNode == NULL ) {
	secondNode = NULL;      // can't look any further
    }
    // get node for second part
    // first, check that 'firstNode' is derived from SoBaseKit.
    // otherwise, 'getPart' is not a valid method, since it doesn't have
    // parts
    else if ( !firstNode->isOfType( SoBaseKit::getClassTypeId() ) ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::getAnyPart",
	"you asked for %s but %s is not a nodeKit, so you can not look inside it for %s. returning NULL",
	nameOfPart.getString(),firstName, remainderString );
#endif
	if ( firstExistedBefore == FALSE ) {
	    // if we just created the node, we'd better get rid of it...
	    setSingleNamePart( firstName, NULL, TRUE );
	}
	secondNode = NULL;
    }
    else {
	// get the second node from within the first...
	// Call the method on the parts list, not the node.
	// It's more efficient and won't do extra virtual things that
	// might be added by subclasses of SoBaseKit.
	SoNodekitParts *interParts = ((SoBaseKit *)firstNode)->nodekitPartsList;
	SbBool secondExistedBefore;
	secondNode = interParts->getAnyPart( remainderString, makeIfNeeded, 
				leafCheck, publicCheck, secondExistedBefore );
	if ( secondNode == NULL && firstExistedBefore == FALSE ) {
	    // if we just created the list, we'd better get rid of it...
	    setSingleNamePart( firstName, NULL, TRUE );
	}
	existedBefore = firstExistedBefore && secondExistedBefore;
    }
    free ( nameCopy );
    return secondNode;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  path to the part asked for
//
// Use: public

SoFullPath *
SoNodekitParts::createPathToAnyPart( const SbName &nameOfPart, 
		    SbBool makeIfNeeded, SbBool leafCheck, 
		    SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    SbBool existedBefore;
    return ( createPathToAnyPart( nameOfPart, makeIfNeeded, leafCheck,
				  publicCheck, existedBefore ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  path to the part asked for
//
// Use: public

SoFullPath *
SoNodekitParts::createPathToAnyPart( const SbName &nameOfPart, 
		    SbBool makeIfNeeded, SbBool leafCheck, 
		    SbBool publicCheck,  SbBool &existedBefore )
//
////////////////////////////////////////////////////////////////////////
{
    existedBefore = FALSE;

    char   *nameCopy, *firstName, *remainderString;
    SoNode *firstNode = NULL;
    SoFullPath *firstPath = NULL, *secondPath = NULL, *answer = NULL;

    // JUST A SINGLE NAME...
    if ( strrchr( nameOfPart.getString(), '.' ) == NULL )
	return getSingleNamePathToPart( nameOfPart, makeIfNeeded, 
				    leafCheck, publicCheck, existedBefore );

    // OTHERWISE, A COMPOUND NAME of the form:  part1.part2.part3 etc.
    // copy the string
    nameCopy = strdup( nameOfPart.getString() );

    // break string into two parts
    firstName = strtok( nameCopy, ".");   // everything before the first '.'
    remainderString = strtok( NULL, "");  // everything else

    // get node for first part
    // intermediate names have to be leaves, so arg3 (leafCheck) is TRUE
    SbBool firstExistedBefore;
    firstPath = getSingleNamePathToPart( firstName, makeIfNeeded, TRUE, 
					 publicCheck, firstExistedBefore );
    if ( firstPath == NULL ) {
	answer = NULL;
    }
    else {

	firstPath->ref();

	firstNode = firstPath->getTail();

	if ( firstNode == NULL ) {
	    answer = NULL;      // can't look any further
	}
	// get node for second part
	// first, check that 'firstNode' is derived from SoBaseKit.
	// otherwise, 'getPart' is not a valid method, since it doesn't have
	// parts
	else if ( !firstNode->isOfType( SoBaseKit::getClassTypeId() ) ) {
#ifdef DEBUG
	    SoDebugError::post( "SoNodekitParts::createPathToAnyPart",
	    "you asked for %s but %s is not a nodeKit, so you can not look inside it for %s. returning NULL",
	    nameOfPart.getString(), firstName, remainderString );
#endif
	    if ( firstExistedBefore == FALSE ) {
		// if we just created the node, we'd better get rid of it...
		setSingleNamePart( firstName, NULL, TRUE );
	    }
	    answer = NULL;
	}
	else {
	    // next, build a path from the second node to the end...
	    // Call the method on the parts list, not the node.
	    // It's more efficient and won't do extra virtual things that
	    // might be added by subclasses of SoBaseKit.
	    SoNodekitParts *interParts 
		= ((SoBaseKit *)firstNode)->nodekitPartsList;
	    SbBool secondExistedBefore;
	    secondPath = (SoFullPath *)
		interParts->createPathToAnyPart( remainderString, makeIfNeeded,
				leafCheck, publicCheck, secondExistedBefore );
	    if ( secondPath )
		secondPath->ref();
	    answer = addPaths( firstPath, secondPath );
	    if ( secondPath == NULL && firstExistedBefore == FALSE ) {
		// if we just created the list, we'd better get rid of it...
		setSingleNamePart( firstName, NULL, TRUE );
	    }
	    existedBefore = firstExistedBefore && secondExistedBefore;
	}
    }
    // get rid of paths...
    if ( firstPath )
	firstPath->unref();
    if ( secondPath )
	secondPath->unref();
    free ( nameCopy );
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the given part with the node passed in as an argument.
//    Returns TRUE if successful, FALSE if not.
//
// Use: private

SbBool
SoNodekitParts::setAnyPart( const SbName &nameOfPart, SoNode *newPartNode, 
			    SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    char   *nameCopy, *firstName, *remainderString;
    SoNode *firstNode = NULL;
    SbBool answer;

    // JUST A SINGLE NAME...
    if ( strrchr( nameOfPart.getString(), '.' ) == NULL )
	return setSingleNamePart( nameOfPart, newPartNode, anyPart );

    // OTHERWISE, A COMPOUND NAME of the form:  part1.part2.part3 etc.
    // copy the string
    nameCopy = strdup( nameOfPart.getString() );

    // break string into two parts
    firstName = strtok( nameCopy, ".");   // everything before the first '.'
    remainderString = strtok( NULL, "");  // everything else

    // Get firstNode:
    // [1] Only need to create it if (newPartNode != NULL)
    //     This is because, if new part is to be NULL, then the
    //     nodekit containing it need not exist either.
    // [2] intermediate names have to be leaves, so arg3 (leafCheck) is TRUE
    SbBool needToMake = ( newPartNode != NULL);
    SbBool firstExistedBefore;
    firstNode = getSingleNamePart( firstName, needToMake, 
				   TRUE, !anyPart, firstExistedBefore );

    if ( firstNode == NULL && needToMake == FALSE ) {
	answer = TRUE;
    }
    else if ( firstNode == NULL ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::setAnyPart",
	"cannot set the part %s because the sub-part %s cannot be made",
				nameOfPart.getString(),firstName );
#endif
	answer = FALSE;
    }
    // call setAnyPart within firstNode...
    // but first, check that 'firstNode' is derived from SoBaseKit.
    // otherwise, 'setAnyPart' is not a valid method, since it doesn't have
    // parts
    else if ( !firstNode->isOfType( SoBaseKit::getClassTypeId() ) ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::setAnyPart",
	"you asked for %s but %s is not a nodeKit, so you can not look inside it for %s. returning NULL",
	nameOfPart.getString(), firstName, remainderString );
#endif
	if ( firstExistedBefore == FALSE ) {
	    // if we just created the node, we'd better get rid of it...
	    setSingleNamePart( firstName, NULL, TRUE );
	}
	answer = FALSE;
    }
    else {
	// call setAnyPart within firstNode...
	// Call the method on the parts list, not the node.
	// It's more efficient and won't do extra virtual things that
	// might be added by subclasses of SoBaseKit.
	SoNodekitParts *interParts 
	    = ((SoBaseKit *)firstNode)->nodekitPartsList;
	answer = interParts->setAnyPart(remainderString, newPartNode, anyPart );

	if ( answer == FALSE && firstExistedBefore == FALSE ) {
	    // if we just created the list, we'd better get rid of it...
	    setSingleNamePart( firstName, NULL, TRUE );
	}
    }

    free ( nameCopy );
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds list from 'this' down to 'theNode'
//    Note:
//       Assumes that 'nameOfPart' 'partNum', and 'theNode' have already
//       been determined to correctly correlate within this partsList
//
// Use: private

SoFullPath *
SoNodekitParts::createPathDownTo( const SbName &nameOfPart, SoNode *theNode )
//
////////////////////////////////////////////////////////////////////////
{
    if ( theNode == NULL )
	return NULL;

    // construct a path from 'this' to the given part.
    SoNode    *childNode = NULL;
    SoGroup   *parentNode = NULL;
    SoBaseKit *thisNode = NULL;
    int     childPartNum, parentPartNum, count;
    int     *backwardsKidIndexArray;

    // allocate enough room to hold all the entries...
    backwardsKidIndexArray = new int[numEntries];

    thisNode      = rootPointer;
    childPartNum  = catalog->getPartNumber( nameOfPart );
    childNode     = theNode;

    // go backwards up the catalog to the top, saving the child index
    // at each step of the way.
    for ( count = 0; childNode != thisNode; count++ ) {
	parentPartNum = catalog->getParentPartNumber( childPartNum );
	if ( parentPartNum != SO_CATALOG_THIS_PART_NUM ) {
	    parentNode = (SoGroup *) fieldList[ parentPartNum ]->getValue();
	    if ( !parentNode->isOfType( SoGroup::getClassTypeId() ) ) {
#ifdef DEBUG
		SoDebugError::post("NodekitParts::createPathDownTo",
		"Parent part not derived from a group");
#endif
		return NULL;
	    }
	    backwardsKidIndexArray[ count ] = parentNode->findChild( childNode);
	    childPartNum = parentPartNum;
	    childNode = parentNode;
	}
	else {
	    backwardsKidIndexArray[ count ] = thisNode->findChild( childNode );
	    childNode = thisNode;
	}
    }

    SoFullPath *answer = (SoFullPath *) new SoPath( rootPointer );
    answer->ref();
    for( int i = count - 1; i >= 0; i-- ) {
	answer->append( backwardsKidIndexArray[i] );
    }

    delete [ /*numEntries*/ ] backwardsKidIndexArray;
    answer->unrefNoDelete();
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Appends pathB after pathA and returns the result.
//    Returns NULL if either path is NULL, or if
//    pathA->getTail() != pathB->getHead()
//
// Use: private

SoFullPath *
SoNodekitParts::addPaths( const SoFullPath *pathA, 
			  const SoFullPath *pathB ) const
//
////////////////////////////////////////////////////////////////////////
{
    if ( pathA == NULL )
	return NULL;

    if ( pathB == NULL )
	return NULL;

    if ( pathA->getTail() != pathB->getHead() )
	return NULL;

    // copy pathA into answer
    SoFullPath *answer = (SoFullPath *) pathA->copy();
    answer->ref();

    // append entries in pathB, but leave out the head, since it already
    // matches the tail of pathA
    for( int i = 1; i < pathB->getLength(); i++ ) {
	answer->append( pathB->getIndex( i ) );
    }
    answer->unrefNoDelete();
    return answer;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    parses the given string into a list name and an index.
//    destructive to parseString, so give it a copy.
//    Syntax for string is:   
//               nameOfList[elementNum]
//    Returns TRUE if successful, FALSE if not.
//
// Use: private

SbBool
SoNodekitParts::parseListItem( char *parseString, int &index ) 
//
////////////////////////////////////////////////////////////////////////
{
    // First call to strtok writes over "[" with end-of-string character.
	strtok( parseString, "["); // look for array bracket
    char *indexString = strtok( NULL, "]");  // look for array close bracket

    if ( indexString == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitParts::parseListItem",
			   "Can\'t parse the name: %s", parseString );
#endif
	return FALSE;
    }
    sscanf( indexString, "%d", &index );    // read index from string
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Determines if partNum is for a non-leaf part that can be deleted.
//    This will be TRUE if the part is:
//     [a] an actual part.
//     [b] not "this"
//     [c] not a leaf.
//     [d] non-NULL
//     [e1] an SoGroup with no children 
//     OR 
//     [e2] an SoSeparator with no children and default field values.
//
SbBool
SoNodekitParts::partIsNonLeafAndMayBeDeleted( int partNum )
{
    // [a] an actual part.
    if ( partNum == SO_CATALOG_NAME_NOT_FOUND )
	return FALSE;

    // [b] not "this"
    if (partNum == SO_CATALOG_THIS_PART_NUM)
	return FALSE;

    // [c] not a leaf.
    if ( catalog->isLeaf( partNum ) == TRUE )
	return FALSE;

    // [d] non-NULL
    SoNode  *part = fieldList[partNum]->getValue();
    if (  part == NULL )
	return FALSE;

    // [e1] an SoGroup with no children 
    if (part->getTypeId() == SoGroup::getClassTypeId() ) {
	if ( ((SoGroup *)part)->getNumChildren() == 0 )
	    return TRUE;
	else
	    return FALSE;
    }

    // OR 
    // [e2] an SoSeparator with no children and default field values.
    if ( part->getTypeId() == SoSeparator::getClassTypeId()) {
	SoSeparator *sep = (SoSeparator *) part;
	if ( sep->getNumChildren() != 0 )
	    return FALSE;
	if ( ! sep->renderCaching.isDefault() &&
	       sep->renderCaching.getValue() != SoSeparator::AUTO )
	    return FALSE;
	if ( ! sep->boundingBoxCaching.isDefault() &&
	       sep->boundingBoxCaching.getValue() != SoSeparator::AUTO )
	    return FALSE;
	if ( ! sep->renderCulling.isDefault() &&
	       sep->renderCulling.getValue() != SoSeparator::AUTO )
	    return FALSE;
	if ( ! sep->pickCulling.isDefault() &&
	       sep->pickCulling.getValue() != SoSeparator::AUTO )
	    return FALSE;

	// Sep has no children and default values, so go ahead and delete.
	return TRUE;
    }

    // If we got here, it's neither a group nor a separator.
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    prints error if partNum is not legal
//
SbBool
SoNodekitParts::partFoundCheck( int partNum )
{
    if ( partNum == SO_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::partFoundCheck",
			    "Can\'t find part");
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    prints error if partNum is not a leaf
//
SbBool
SoNodekitParts::partIsLeafCheck( int partNum )
{
    if ( catalog->isLeaf( partNum ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::partIsLeafCheck",
	"can't return the part %s because it is not a leaf node in the nodekit\'s structure. Returning NULL instead",
			   catalog->getName( partNum ).getString() );
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    prints error if partNum is not public
//
SbBool
SoNodekitParts::partIsPublicCheck( int partNum )
{
    if ( catalog->isPublic( partNum ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::partIsPublicCheck",
	"can\'t return the part %s because it is not a public node in the nodekit's structure returning NULL instead",
			   catalog->getName( partNum ).getString() );
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This checks that a part still exists as the child of its parent,
//    and so on up to 'this'
//    It needs to be called as protection against outside parties deciding to
//    delete nodes in the kit.
//    If the integrity of the kit is broken, routines which call
//    'checkPartIntegrity' will know to create new parts instead, or
//    perhaps return NULL.
//
SbBool
SoNodekitParts::verifyPartExistence( int partNum )
{
    if (partNum == SO_CATALOG_THIS_PART_NUM) {
	if ( rootPointer )
	    return TRUE;
	else {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitParts::verifyPartExistence",
	"the part \"this\" is NULL, and should NEVER be. Expect a core dump");
#endif
	    return FALSE;
	}
    }

    SoNode  *part = fieldList[partNum]->getValue();

    // is the part there?
    if ( part == NULL )
	return FALSE;

    // is the part the top of the catalog? Then we're OK
    if ( partNum == SO_CATALOG_THIS_PART_NUM )
	return TRUE;
    
    int parentPartNum = catalog->getParentPartNumber( partNum );
    SoNode *parent = NULL;
    if (parentPartNum == SO_CATALOG_THIS_PART_NUM)
        parent = rootPointer;
    else
        parent = fieldList[parentPartNum]->getValue();

    // parent should exist.
    if ( parent == NULL )
	return FALSE;

    // part should be a valid child of parent
    if ( parentPartNum != SO_CATALOG_THIS_PART_NUM &&   
	 ((SoGroup *)parent)->findChild( part ) < 0 )
	    return FALSE;
    else if ( parentPartNum == SO_CATALOG_THIS_PART_NUM  
	      && ((SoBaseKit *)parent)->findChild( part ) < 0 )
	    return FALSE;

    // verify the parent
    return( verifyPartExistence( parentPartNum ) );
}
