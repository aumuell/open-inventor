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
 |      SoV1NodekitParts
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1NodekitParts.h>
#include <Inventor/misc/upgraders/SoV1BaseKit.h>
#include <Inventor/SoPath.h>
#include <Inventor/errors/SoDebugError.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoV1NodekitParts
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoV1NodekitParts::SoV1NodekitParts( SoV1BaseKit *rootOfKit,
				SoV1NodekitParts *partsSoFar )
//
////////////////////////////////////////////////////////////////////////
{
    catalog = rootOfKit->getNodekitCatalog();  // assign the catalog

    numEntries = catalog->getNumEntries();        // make an empty node list
    nodeList = new ( SoNode *[numEntries] );
    int i;
    for ( i = 0; i < numEntries; i++ )
	nodeList[i] = NULL;

    // make rootOfKit be the entry in the nodeList for 'this'
    const int partNum = catalog->getPartNumber( "this" );
    if ( partNum == SO_V1_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::SoV1NodekitParts", 
			   "can't find entry for top node in catalog");
#endif
    }
    else if ( rootOfKit == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::SoV1NodekitParts", 
			   "given root node is NULL");
#endif
    }
    else if (catalog->getType(partNum) != rootOfKit->getTypeId()){
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::SoV1NodekitParts", 
			   "given root node is of wrong type");
#endif
    }
    else  {
	nodeList[partNum] = rootOfKit;
    }

    // copy parts that already exist in 'partsSoFar' into this
    // partsList
    if ( partsSoFar != NULL ) {
	for (i = 0; i < partsSoFar->numEntries; i++ ) {
	    if (    partsSoFar->nodeList[i] != NULL 
		 && partsSoFar->nodeList[i] != rootOfKit ) {

		 // the part exists already in 'partsSoFar'
		 // find the entry in the newly constructed list that
		 // corresponds to it.

		 // first, get name from 'partsSoFar'
		 const SbName partName = partsSoFar->catalog->getName( i );

		 // now, find corresponding entry in THIS list
		 const int oldPartNum = catalog->getPartNumber( partName );

		 if ( oldPartNum != SO_V1_CATALOG_NAME_NOT_FOUND )
		    nodeList[oldPartNum] = partsSoFar->nodeList[i];
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

SoV1NodekitParts::~SoV1NodekitParts()
//
////////////////////////////////////////////////////////////////////////
{
    // delete the nodelist
    delete [ /*numEntries*/ ] nodeList;
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
SoV1NodekitParts::makePart( const int partNum )
//
////////////////////////////////////////////////////////////////////////
{
    if ( !partFoundCheck( partNum ) )
	return FALSE;

    // if the part is already in the Node list, just return TRUE
    if ( verifyPartExistence( partNum ) )
	return TRUE;

    // create the node
    const SoNode *inst = (const SoNode *) 
			    catalog->getDefaultType( partNum ).createInstance();
#ifdef DEBUG
    if ( inst == NULL ) {
	SoDebugError::post("SoV1NodekitParts::makePart", 
			   "Can't make part %s. It belongs to an abstract class. Bad parts catalog", catalog->getName(partNum).getString());
	abort();
    }
#endif
    nodeList[partNum] = (SoNode *) inst;

    nodeList[partNum]->ref();  // temporarily ref it, until it has a parent

    // if it is the topmost part in the nodekit, just return TRUE
    if ( catalog->getParentName( partNum ) == "" ) {
	nodeList[partNum]->unref();  // undo the temporary ref
	return TRUE;
    }

    // create its parent part
    if ( !makePart( catalog->getParentPartNumber( partNum ) ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::makePart", 
			   "can't make parent for part named %s",
			    catalog->getName(partNum).getString() );
#endif
	nodeList[partNum]->unref();  // undo the temporary ref
	return FALSE;
    }
    int parentPartNum = catalog->getParentPartNumber( partNum );

    // find the next closest right sibling that already exists
    int    sibPartNum, searchPartNum;

    for( sibPartNum = -1, 
	 searchPartNum = catalog->getRightSiblingPartNumber( partNum );
	 sibPartNum == -1 && searchPartNum != SO_V1_CATALOG_NAME_NOT_FOUND;
	 searchPartNum = catalog->getRightSiblingPartNumber(searchPartNum )) {
	if ( verifyPartExistence( searchPartNum ) )
	    sibPartNum = searchPartNum;
    }

    SoGroup *parentNode = (SoGroup *) nodeList[parentPartNum];
    if (sibPartNum == -1 ) {
	// no right sibling is made yet, so just add this as a child to parent
	parentNode->addChild( nodeList[partNum] );
    }
    else {
	// find that sibling's index in the parent
	int sibIndex = parentNode->findChild(nodeList[sibPartNum]);

	// insert this node as the new child at that index
	parentNode->insertChild( nodeList[partNum], sibIndex );
    }
    nodeList[partNum]->unref();  // undo the temporary ref
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
SoV1NodekitParts::replacePart( const int partNum, SoNode *newPartNode )
//
////////////////////////////////////////////////////////////////////////
{
    if ( !partFoundCheck( partNum ) )
	return FALSE;

    int parentPartNum = catalog->getParentPartNumber( partNum );

    // make sure the node given is of the proper type
    if ( newPartNode != NULL
	 && !newPartNode->isOfType( catalog->getType( partNum ) ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::replacePart", 
			   "the given part is not of the correct type");
#endif
	return FALSE;
    }

    // if an old part is already in the Node list...
    if ( verifyPartExistence( partNum ) ) {
	// disconnect the child from its parent
	((SoGroup *)nodeList[parentPartNum])->removeChild( nodeList[partNum] );
    }

    // set the part!
    nodeList[partNum] = newPartNode;

    // if we only set the node to NULL, then just return
    if ( newPartNode == NULL)
	return TRUE;

    // if it is the topmost part in the nodekit, just return TRUE
    if ( catalog->getParentName( partNum ) == "" )
	return TRUE;

    // create its parent part
    if ( !makePart( catalog->getParentPartNumber( partNum ) ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::replacePart", 
			   "can't make parent for part named %s",
			    catalog->getName(partNum).getString() );
#endif
	return FALSE;
    }

    // find the next closest right sibling that already exists
    int sibPartNum, searchPartNum;

    for( sibPartNum = -1, 
	 searchPartNum = catalog->getRightSiblingPartNumber( partNum );
	 sibPartNum == -1 && searchPartNum != SO_V1_CATALOG_NAME_NOT_FOUND;
	 searchPartNum = catalog->getRightSiblingPartNumber( searchPartNum)) {
	if ( verifyPartExistence( searchPartNum ) )
	    sibPartNum = searchPartNum;
    }

    SoGroup *parentNode = (SoGroup *) nodeList[parentPartNum];
    if (sibPartNum == -1 ) {
	// no right sibling is made yet, so just add this as a child to parent
	parentNode->addChild( nodeList[partNum] );
    }
    else {
	// find that sibling's index in the parent
	int sibIndex = parentNode->findChild(nodeList[sibPartNum]);

	// insert this node as the new child at that index
	parentNode->insertChild( nodeList[partNum], sibIndex );
    }
    return TRUE;
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
SoV1NodekitParts::getPartFromThisCatalog( const int partNum, 
				  SbBool makeIfNeeded, SbBool leafCheck, 
				  SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    if ( !partFoundCheck( partNum ) )
	return FALSE;

    if ( leafCheck ) {
	if ( !partIsLeafCheck( partNum ) )
	    return FALSE;
    }
    if ( publicCheck ) {
	if ( !partIsPublicCheck( partNum ) )
	    return FALSE;
    }

// Don't do this check any more. For this reduced version of SoBaseKit,
// allow parts that are lists to be returned with getAnyPart()
//XXX    if ( !partIsNotListCheck( partNum ) )
//XXX	return FALSE;

    if ( makeIfNeeded == FALSE ) {
	// just return whatever you can find
	if ( partNum != SO_V1_CATALOG_NAME_NOT_FOUND 
	     && verifyPartExistence(partNum) )
	    return ( nodeList[ partNum ] );
	else
	    return NULL;
    }

    // otherwise, we need to make the part
    if ( makePart( partNum ) == TRUE )
	return ( nodeList[ partNum ] );   // it was made O.K.
    else
	return NULL;                      // it didn't get made properly
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
SoV1NodekitParts::setPartFromThisCatalog( const int partNum, 
				         SoNode *newPartNode, SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    if ( !partFoundCheck( partNum ) )
	return FALSE;
    if ( !anyPart ) {
	if ( !partIsLeafCheck( partNum ) )
	    return FALSE;
	if ( !partIsPublicCheck( partNum ) )
	    return FALSE;
	if ( !partIsNotListCheck( partNum ) )
	    return FALSE;
    }

    // otherwise, we need to replace the part that is currently being used
    return( replacePart( partNum, newPartNode ) );
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets  the part asked for
//
// Use: protected

SoNode *
SoV1NodekitParts::getAnyPart( const SbName &nameOfPart, SbBool makeIfNeeded, 
			    SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    // ONLY ALLOW SINGLE NAME PARTS...
    if ( strrchr( nameOfPart.getString(), '.' ) != NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::getAnyPart", 
			   "part name %s is not a single-word part",
			   nameOfPart.getString());
#endif
	return NULL;
    }

    // DON'T ALLOW BRACKETS, WHICH SIGNIFY INDEXING INTO A LIST
    // IN THE NEW NODEKITS.
    if ( strrchr( nameOfPart.getString(), '[') != NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::getAnyPart", 
			   "part name %s contains an index, which is illegal",
			   nameOfPart.getString());
#endif
	return FALSE;
    }

    // IS THE REQUESTED PART IN THIS CATALOG?
    int partNum = catalog->getPartNumber( nameOfPart );
    if ( partNum != SO_V1_CATALOG_NAME_NOT_FOUND )

	// IF SO, THEN GET IT FROM THIS CATALOG
	return ( getPartFromThisCatalog( partNum, makeIfNeeded,leafCheck, 
					    publicCheck));

    else {

	// ELSE, SEARCH THE CATALOG RECURSIVELY FOR THE DESIRED PART
	// we need to pass a list to the recursive search saying which
	// types of nodes we have already checked.  This avoids infinite
	// loop (does chicken contain egg? does egg contain chicken? etc...)
	SbPList *typesChecked = new SbPList();
	int thisPartNum = catalog->getPartNumber( "this" );
	typesChecked->append( 
	    (void *) catalog->getType(thisPartNum).getName().getString() );

	for (int i = 0; i < numEntries; i++ ) {

	    // does it lie within this 'intermediary' part?
	    if ( catalog->recursiveSearch(i,nameOfPart,typesChecked) == TRUE){

		delete typesChecked;  // don't need this anymore

		// if not making parts and 'intermediary' is NULL...
		if ( makeIfNeeded == FALSE && !verifyPartExistence( i ) )
		    return NULL;

		// create the intermediary part...
		if ( !makePart( i ) )
		    return NULL;   // error making the part
		// NOTE: it's okay to cast the node into a base kit here,
		// since the recursive search would only have worked
		// if it was a nodekit
		// UGLY, yes, but it works.
#ifdef DEBUG
		// supposedly unnecessary type checking:
		if ( !nodeList[i]->isOfType( SoV1BaseKit::getClassTypeId() ) )
		    return NULL;
#endif
		SoV1BaseKit *intermediary = (SoV1BaseKit *) nodeList[i]; 

		// now that intermediary is built, get the part from within it
		return(intermediary->getAnyPart(nameOfPart,
				    SoNode::getClassTypeId(), makeIfNeeded, 
				    leafCheck, publicCheck ));
	    }
	}
	delete typesChecked;  // don't need this anymore

    }

    // IF YOU GOT HERE, THE PART COULD NOT BE FOUND
#ifdef DEBUG
    SoDebugError::post("SoV1NodekitParts::getAnyPart", 
			   "entry named %s not found. returning NULL", 
			   nameOfPart.getString() );
#endif
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the given part with the node passed in as an argument.
//    Returns TRUE if successful, FALSE if not.
//
// Use: private

SbBool
SoV1NodekitParts::setAnyPart( const SbName &nameOfPart, SoNode *newPartNode, 
			    SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    // JUST SINGLE NAME PARTS...
    if ( strrchr( nameOfPart.getString(), '.' ) != NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::setAnyPart", 
			   "part name %s is not a single-word part",
			   nameOfPart.getString());
#endif
	return FALSE;
    }

    // DON'T ALLOW BRACKETS, WHICH SIGNIFY INDEXING INTO A LIST
    // IN THE NEW NODEKITS?
    if ( strrchr( nameOfPart.getString(), '[') != NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::setAnyPart", 
			   "part name %s contains an index, which is illegal",
			   nameOfPart.getString());
#endif
	return FALSE;
    }

    // NOT A LIST ITEM, IF WE GOT HERE.

    // IS THE REQUESTED PART IN THIS CATALOG?
    int partNum = catalog->getPartNumber( nameOfPart );
    if ( partNum != SO_V1_CATALOG_NAME_NOT_FOUND )

	// IF SO, THEN SET IT USING THIS CATALOG
	return ( setPartFromThisCatalog( partNum, newPartNode, anyPart ) );

    else {

	// ELSE, SEARCH THE CATALOG RECURSIVELY FOR THE DESIRED PART
	// we need to pass a list to the recursive search saying which
	// types of nodes we have already checked.  This avoids infinite
	// loop (does chicken contain egg? does egg contain chicken? etc...)
	SbPList *typesChecked = new SbPList();
	int thisPartNum = catalog->getPartNumber( "this" );
	typesChecked->append( 
	    (void *) catalog->getType(thisPartNum).getName().getString() );

	for (int i = 0; i < numEntries; i++ ) {

	    // does it lie within this 'intermediary' part?
	    if ( catalog->recursiveSearch(i,nameOfPart,typesChecked) == TRUE){

		delete typesChecked;  // don't need this anymore

		// create the intermediary part...
		if ( !makePart( i ) )
		    return FALSE;   // error making the part
		// NOTE: it's okay to cast the node into a base kit here,
		// since the recursive search would only have worked
		// if it was a nodekit
		// UGLY, yes, but it works.
#ifdef DEBUG
		// supposedly unnecessary type checking:
		if ( !nodeList[i]->isOfType( SoV1BaseKit::getClassTypeId() ) )
		    return FALSE;
#endif
		SoV1BaseKit *intermediary = (SoV1BaseKit *) nodeList[i]; 

		// now that intermediary is built, set the part within it
		return(intermediary->setAnyPart(nameOfPart, newPartNode, 
						anyPart ));
	    }
	}
	delete typesChecked;  // don't need this anymore

    }

    // IF YOU GOT HERE, THE PART COULD NOT BE FOUND
#ifdef DEBUG
    SoDebugError::post("SoV1NodekitParts::setAnyPart", 
		       "entry named %s not found. returning NULL", 
		       nameOfPart.getString() );
#endif
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    error if partNum is not legal
//
SbBool
SoV1NodekitParts::partFoundCheck( int partNum )
{
    if ( partNum == SO_V1_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::partFoundCheck", 
			   "can't find part");
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    error if partNum is not a leaf
//
SbBool
SoV1NodekitParts::partIsLeafCheck( int partNum )
{
    if ( catalog->isLeaf( partNum ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::partIsLeafCheck", 
			   "can't return the part %s because it is not a leaf node in the nodekit's structure. returning NULL",
			   catalog->getName( partNum ).getString() );
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    error if partNum is not public
//
SbBool
SoV1NodekitParts::partIsPublicCheck( int partNum )
{
    if ( catalog->isPublic( partNum ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::partIsPublicCheck", 
	    "can't return the part %s because it is not a public node in the nodekit's structure returning NULL instead",
			   catalog->getName( partNum ).getString() );
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    error if partNum is not a list
//
SbBool
SoV1NodekitParts::partIsNotListCheck( int partNum )
{
    if ( catalog->isList( partNum ) == TRUE ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitParts::partIsNotListCheck", 
	    "can't return the part %s because it is a list in the nodekit's structure Lists must be accessed throught the methods in the nodekit. returning NULL instead",
			   catalog->getName(partNum).getString() );
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
SoV1NodekitParts::verifyPartExistence( int partNum )
{
    SoNode  *part = nodeList[partNum];

    // is the part there?
    if ( part == NULL )
	return FALSE;

    // is the part the top of the catalog? Then we're OK
    if ( partNum == catalog->getPartNumber( "this" ) )
	return TRUE;
    
    int parentPartNum = catalog->getParentPartNumber( partNum );
    SoGroup *parent = (SoGroup *) nodeList[parentPartNum];

    // parent should exist.
    if ( parent == NULL )
	return FALSE;

    // part should be a valid child of parent
    if ( parent->findChild( part ) < 0 )
	return FALSE;

    // verify the parent
    return( verifyPartExistence( parentPartNum ) );
}
