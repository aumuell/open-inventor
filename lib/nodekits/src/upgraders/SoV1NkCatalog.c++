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
 |      SoV1NodekitCatalog
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/misc/upgraders/SoV1NodekitCatalog.h>
#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1BaseKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/errors/SoDebugError.h>

// Static variables for the nodekit catalog class...

const SbName  *SoV1NodekitCatalog::emptyName = NULL;
const SbPList *SoV1NodekitCatalog::emptyList = NULL;
SoType  *SoV1NodekitCatalog::badType   = NULL;

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoV1NodekitCatalogEntry
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoV1NodekitCatalogEntry::SoV1NodekitCatalogEntry( const SbName &theName, 
			const SoType  &theType , const SoType &theDefaultType,
			const SbName  &theParentName, 
			const SbName  &theRightSiblingName, SbBool theListPart, 
			const SbPList &theListItemTypes,
			      SbBool thePublicPart )
//
////////////////////////////////////////////////////////////////////////
{
    name                   = theName;
    type                   = theType;
    defaultType            = theDefaultType;
    leafPart               = TRUE;    // everything is a leaf 'til given a child
    parentName             = theParentName;
    rightSiblingName       = theRightSiblingName;
    listPart               = theListPart;
    listItemTypes.copy( theListItemTypes );
    publicPart             = thePublicPart;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Print contents for debugging purposes
//
// Use: internal

void 
SoV1NodekitCatalogEntry::printCheck() const
//
////////////////////////////////////////////////////////////////////////
{
    fprintf( stdout,"    name = %s, type = %d, defaultType = %d\n",
	    name.getString(), type, defaultType );
    fprintf( stdout,"    parentName = %s\n", parentName.getString() );
    fprintf( stdout,"    sibling = %s, listPart = %d\n",
	    rightSiblingName.getString(), listPart );
    if ( listPart ) {
	fprintf( stdout, "listItemTypes = " );
	for ( int i = 0; i < listItemTypes.getLength(); i++ ) {
	    fprintf( stdout,"  %d  ", 
		     ((SoType *) listItemTypes[i])->getName().getString() );
	}
	fprintf( stdout, "\n" );
    }
    fprintf( stdout,"    publicPart = %d\n", publicPart );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog
//
// Use: private

SoV1NodekitCatalogEntry *
SoV1NodekitCatalogEntry::clone() const
//
////////////////////////////////////////////////////////////////////////
{
    // make a clone with the current type and defaultType...
    return clone( type, defaultType );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog, but sets the type to newType
//
// Use: private

SoV1NodekitCatalogEntry *
SoV1NodekitCatalogEntry::clone( const SoType &newType, 
			      const SoType &newDefaultType ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoV1NodekitCatalogEntry *theClone;

    theClone = new SoV1NodekitCatalogEntry( name, newType, newDefaultType,
	 parentName, rightSiblingName, listPart, listItemTypes, publicPart );
    theClone->leafPart = leafPart;

    return theClone;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog
//
// Use: public

void
SoV1NodekitCatalogEntry::addListItemType( const SoType &typeToAdd )
//
////////////////////////////////////////////////////////////////////////
{
    SoType *typePtr = new SoType();
    *typePtr = typeToAdd;
    listItemTypes.append( typePtr );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks for the given part.
//    Checks this part, then recursively checks all entries in the 
//    catalog of this part (it gets its own catalog by looking
//    at the catalog for the 'dummy' for this type, which is accessed
//    through: type.createInstance()->getNodekitCatalog()
//    or, if type is an abstract type, 
//    uses defaultType.createInstance()->getNodekitCatalog()
//
// Use: public

SbBool
SoV1NodekitCatalogEntry::recursiveSearch( const SbName    &nameToFind, 
					  SbPList   *typesChecked ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoV1NodekitCatalog *subCat;

    // is this the part of my dreams?
    if ( name == nameToFind )
	return TRUE;

    // make sure the part isn't a list
    if ( listPart == TRUE )
	return FALSE;
    // make sure the part is subclassed off of SoV1BaseKit
    if ( !type.isDerivedFrom( SoV1BaseKit::getClassTypeId() ))
	return FALSE;

    // avoid an infinite search loop by seeing if this type has already been
    // checked...
    if ( typesChecked->find( (void *) type.getName().getString()) != -1 )
	return FALSE;

    // if it's still ok, then search within the catalog of this part
    // first check each name:
    const SoV1BaseKit *inst = (const SoV1BaseKit *) type.createInstance();
    if ( inst == NULL )
	inst = (const SoV1BaseKit *) defaultType.createInstance();
#ifdef DEBUG
    if ( inst == NULL ) {
	SoDebugError::post("SoV1NodekitCatalogEntry::recursiveSearch",
	    "part type and defaultType are both abstract classes");
	abort();
    }
#endif

    subCat = inst->getNodekitCatalog();
    int i;
    for( i = 0; i < subCat->getNumEntries(); i++ ) {
	if ( subCat->getName( i ) == nameToFind ) 
		return TRUE;
    }
    // at these point, we've checked all the names in this class, so 
    // we can add it to typesChecked
    typesChecked->append( (void *) type.getName().getString() );

    // then, recursively check each part
    for( i = 0; i < subCat->getNumEntries(); i++ ) {
	if ( subCat->recursiveSearch( i, nameToFind, typesChecked ) )
		return TRUE;
    }

    return FALSE;  // couldn't find it ANYwhere!
}


////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoV1NodekitCatalog
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoV1NodekitCatalog::SoV1NodekitCatalog()
//
////////////////////////////////////////////////////////////////////////
{
    numEntries = 0;
    entries    = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: internal

SoV1NodekitCatalog::~SoV1NodekitCatalog()
//
////////////////////////////////////////////////////////////////////////
{
    // delete all the members of the entries array
    for ( int i = 0; i < numEntries; i++ )
	delete entries[i];

    // delete the entries array itself
    if (entries != NULL)
	delete entries;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints out contents of the catalog for debuggin purposes
//
// Use: internal

void
SoV1NodekitCatalog::printCheck() const
//
////////////////////////////////////////////////////////////////////////
{
    fprintf(stdout,"catalog printout: number of entries = %d\n", numEntries );
    for( int i = 0; i < numEntries; i++ ) {
	fprintf(stdout,"#%d\n", i );
	entries[i]->printCheck();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the partNumber of an entry given its reference name.
//
// Use: internal

int
SoV1NodekitCatalog::getPartNumber( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    void *castPNum;

    if ( partNameDict.find( (unsigned long) theName.getString(), castPNum ) )
#if (_MIPS_SZPTR == 64)
	return ( (int) ((long) castPNum) );  // System long
#else
	return ( (int) castPNum );
#endif
    else 
	return SO_V1_CATALOG_NAME_NOT_FOUND;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of an entry given its partNumber.
//
// Use: internal

const SbName &
SoV1NodekitCatalog::getName( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the name of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getName();
    else
	return *emptyName;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the type of an entry.
//
// Use: internal

const SoType &
SoV1NodekitCatalog::getType( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the type of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getType();
    else
	return *badType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the type of an entry.
//
// Use: internal

const SoType &
SoV1NodekitCatalog::getType( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getType( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the default type of an entry.
//
// Use: internal

const SoType &
SoV1NodekitCatalog::getDefaultType( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the defaultType of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getDefaultType();
    else
	return *badType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the defaultType of an entry.
//
// Use: internal

const SoType &
SoV1NodekitCatalog::getDefaultType( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getDefaultType( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'leafPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isLeaf( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the type of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->isLeaf();
    else
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'leafPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isLeaf( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( isLeaf( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the parent of an entry.
//
// Use: internal

const SbName &
SoV1NodekitCatalog::getParentName( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getParentName();
    else
	return *emptyName;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the parent of an entry.
//
// Use: internal

const SbName &
SoV1NodekitCatalog::getParentName( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getParentName( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the parent of an entry.
//
// Use: internal

int
SoV1NodekitCatalog::getParentPartNumber( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName pName = getParentName( thePartNumber );
    return( getPartNumber( pName ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the parent of an entry.
//
// Use: internal

int
SoV1NodekitCatalog::getParentPartNumber( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName pName = getParentName( theName );
    return( getPartNumber( pName ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the right sibling of an entry.
//
// Use: internal

const SbName &
SoV1NodekitCatalog::getRightSiblingName( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getRightSiblingName();
    else
	return *emptyName;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the RightSiblingName of an entry.
//
// Use: internal

const SbName &
SoV1NodekitCatalog::getRightSiblingName( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getRightSiblingName( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the rightSibling of an entry.
//
// Use: internal

int
SoV1NodekitCatalog::getRightSiblingPartNumber( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName sName = getRightSiblingName( thePartNumber );
    return( getPartNumber( sName ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of the rightSibling of an entry.
//
// Use: internal

int
SoV1NodekitCatalog::getRightSiblingPartNumber( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName sName = getRightSiblingName( theName );
    return( getPartNumber( sName ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'listPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isList( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the type of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->isList();
    else
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'listPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isList( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( isList( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the list item type of an entry.
//
// Use: internal

const SbPList &
SoV1NodekitCatalog::getListItemTypes( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the type of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getListItemTypes();
    else
	return ( *emptyList );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the list item type of an entry.
//
// Use: internal

const SbPList &
SoV1NodekitCatalog::getListItemTypes( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getListItemTypes( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'publicPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isPublic( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return whether the part is public, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->isPublic();
    else
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'publicPart' of an entry.
//
// Use: internal

SbBool
SoV1NodekitCatalog::isPublic( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( isPublic( getPartNumber( theName ) ));
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    For adding an allowable child-type to an entry.
//    This will only make a difference if the entry has listPart set to TRUE
//
// Use: public

void
SoV1NodekitCatalog::addListItemType( int thePartNumber,
					const SoType &typeToAdd )
//
////////////////////////////////////////////////////////////////////////
{
    // add typeToAdd to the entry's listItemTypes, if you can find
    // the entry...
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	entries[thePartNumber]->addListItemType( typeToAdd );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For adding an allowable child-type to an entry.
//    This will only make a difference if the entry has listPart set to TRUE
//
// Use: public

void
SoV1NodekitCatalog::addListItemType( const SbName &theName,
					const SoType &typeToAdd )
//
////////////////////////////////////////////////////////////////////////
{
    addListItemType( getPartNumber( theName ), typeToAdd );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// For changing the type and defaultType of an entry.
// The new types must be subclasses of the types already existing in the
// entry.
// For example, in SoV1ShapeKit, the part "shape" has type SoShape and
// default type SoSphere.  Any shape node is acceptable, but be default a 
// sphere will be built.
// Well, when creating the SoV1VertexShapeKit class, a call of:
//    narrowTypes( "shape", SoVertexShape::getClassTypeId(),
//                          SoFaceSet::getClassTypeId())
// might be used.  This would reflect the fact that:
// Only vertext shapes may be put in the "shape"  part, not just any shape.
// And also, by default, a faceSet will be built, not a sphere.
//
// Use: public

void
SoV1NodekitCatalog::narrowTypes( const SbName &theName,
			const SoType &newType, const SoType &newDefaultType )
//
////////////////////////////////////////////////////////////////////////
{
    int thePartNumber = getPartNumber( theName );

    if ( thePartNumber < 0 || thePartNumber >= numEntries )
	return;

    SoV1NodekitCatalogEntry *theEntry = entries[thePartNumber];

    // Checks that [1] newDefaultType is not an abstract type, and 
    //             [2] newDefaultType is a subclass of newType
    if ( !checkNewTypes( newType, newDefaultType ) )
	return;

    const SoType oldType        = theEntry->getType();
    const SoType oldDefaultType = theEntry->getDefaultType();

    // Make sure that the new types is derived from the old type.
    // Parts in derived classes must be subclasses of the types they
    // belong to when cast to the parent classes.
    if ( !newType.isDerivedFrom( oldType ) ) {
#ifdef DEBUG
	const char *newName = newType.getName().getString();
	const char *oldName = oldType.getName().getString();
	SoDebugError::post("SoV1NodekitCatalogEntry::narrowTypes",
			   "The newType %s is not a subclass of the oldType "
			   "%s.  Cannot narrow the  type from %s to %s",
			   newName, oldName, oldName, newName);
#endif
  	return;
    }

    // passed all the tests!
    theEntry->setTypes( newType, newDefaultType );
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog
//
// Use: public

SoV1NodekitCatalog *
SoV1NodekitCatalog::clone( const SoType &typeOfThis ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoV1NodekitCatalog      *theClone;

    theClone = new SoV1NodekitCatalog;
    theClone->numEntries = numEntries;
    if (numEntries == 0)
	theClone->entries = NULL;
    else {
	theClone->entries = new ( SoV1NodekitCatalogEntry *[numEntries]);
	for (int i = 0; i < numEntries; i++) {
	    if ( entries[i]->getName() == "this" )
		theClone->entries[i] = entries[i]->clone( typeOfThis, 
							  typeOfThis );
	    else
		theClone->entries[i] = entries[i]->clone();
	    theClone->partNameDict.enter( (unsigned long)
					  entries[i]->getName().getString(), 
					  (void *) i );
	}
    }

    return theClone;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Checks that the name is not NULL and not empty.
//
// Use: private

SbBool
SoV1NodekitCatalog::checkName( const SbName &theName )  // proposed name
//
////////////////////////////////////////////////////////////////////////
{
    // CHECK IF IT'S NULL
    if ( theName.getString() == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkName",
	    "given name is NULL" );
#endif
  	return FALSE;
    }

    // CHECK IF IT'S EMPTY
    if ( theName == "" ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkName",
	    "given name is the empty string" );
#endif
  	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Checks that the name is OK to use for the new entry.
//
// Use: private

SbBool
SoV1NodekitCatalog::checkNewName( const SbName &theName )  // proposed name
//
////////////////////////////////////////////////////////////////////////
{
    if ( !checkName( theName ) )
	return FALSE;

    // CHECK IF IT'S UNIQUE FOR THIS CATALOG
    if ( getPartNumber( theName ) == SO_V1_CATALOG_NAME_NOT_FOUND ) {
	return TRUE;
    }
    else {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkNewName",
	    "the name %s is already in this catalog",
			theName.getString() );
#endif
  	return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Checks that the type is not for an abstract node type.
//
// Use: private

SbBool
SoV1NodekitCatalog::checkNewTypes( const SoType &theType,  // proposed type
				 const SoType &theDefaultType ) // and default
//
////////////////////////////////////////////////////////////////////////
{
    if ( !theDefaultType.canCreateInstance()) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkNewTypes",
	    " the defaultType %s is an abstract class, and cannot be part of a nodekits structure", theDefaultType.getName().getString() );
#endif
  	return FALSE;
    }

    if ( !theDefaultType.isDerivedFrom( theType ) ) {
#ifdef DEBUG
	const char *defName = theDefaultType.getName().getString();
	SoDebugError::post("SoV1NodekitCatalogEntry::checkNewTypes",
			   "the defaultType %s is not a subclass of the type "
			   "%s. It can not be part of a nodekits structure",
			   defName, theType.getName().getString() );
#endif
  	return FALSE;
    }
    else
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Checks that the name is OK to use the parent for a new entry.
//
// Use: private

SbBool
SoV1NodekitCatalog::checkAndGetParent( 
	const SbName                &theName,          // the child's name
	const SbName                &theParentName,    // proposed parent name
	SoV1NodekitCatalogEntry *& parentEntry ) // corresponding entry filled 
					        // in this method
//
////////////////////////////////////////////////////////////////////////
{
    // CHECK FOR "THIS"
    // The string "this" means that a node is to be the nodekit node itself 
    // In this case, return NULL as the parentEntry, and a status of TRUE!
    if ( theName == "this" ) {
	// 'this' has no parent
	parentEntry = NULL;
	return TRUE;
    }
    else {
	// only 'this' can have no parent
	if ( !checkName( theParentName ) )
	    return FALSE;
    }

    // CHECK THAT THE PARENT IS IN THE CATALOG
    int parentPartNumber = getPartNumber( theParentName );
    if ( parentPartNumber == SO_V1_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkAndGetParent",
	    "requested parent >> %s << is not in this catalog",
	    			theParentName.getString() );
#endif
	return FALSE;
    }
    parentEntry = entries[ parentPartNumber ];

    // CHECK THE NODE TYPE OF THE PARENT
    // [1] it must be a subclass of SoGroup, or you can't add children to it.
    if ( !parentEntry->getType().isDerivedFrom( SoGroup::getClassTypeId() ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkAndGetParent",
        "requested parent >> %s << is a node that is not subclassed from SoGroup, so it can't have children",
	    			theParentName.getString() );
#endif
	return FALSE;
    }
    // [2] Unless it is 'this', the parent can NOT be subclass of SoV1BaseKit.
    //     This is because you can only add child nodes to an nodekit through 
    //     its own class's nodekitCatalog
    if ( theParentName != "this" &&
	 parentEntry->getType().isDerivedFrom( SoV1BaseKit::getClassTypeId()) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkAndGetParent",
	    " requested parent >> %s << is a node that is subclassed from SoV1BaseKit, so it can't children except through its own class's nodekitCatalog",
	    			theParentName.getString());
#endif
	return FALSE;
    }

    // MAKE SURE THAT THE NODE HAS NOT BEEN DESIGNATED AS A LIST
    if ( parentEntry->isList() == TRUE ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkAndGetParent",
	    "Requested parent >> %s << is a node that has been designated as a list.  It can not have explicit children in the catalog, although nodes can be added to the list for a given instance by using the special list editting methods on the nodekit node.", theParentName.getString() );
#endif
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Finds the left and right siblings for the new entry.
//    Also, checks that the right sibling exists in the catalog (unless
//    the name is "", in which case the new entry is to be the rightmost child
//
// Use: private

SbBool
SoV1NodekitCatalog::checkAndGetSiblings( 
	const SbName            &theParentName,       // parent name
	const SbName            &theRightSiblingName, // proposed sibling name
	SoV1NodekitCatalogEntry *& leftEntry,      // new left (to be filled in)
	SoV1NodekitCatalogEntry *& rightEntry )    // new right (to be filled )
//
////////////////////////////////////////////////////////////////////////
{
    leftEntry = NULL;
    rightEntry = NULL;

    for ( int i = 0; i < numEntries; i++ ) {
	// is it left?
	if ( entries[i]->getParentName() == theParentName &&
	     entries[i]->getRightSiblingName() == theRightSiblingName )
	     leftEntry = entries[i];
	// is it right?
	else if ( entries[i]->getParentName() == theParentName &&
	     theRightSiblingName == entries[i]->getName() )
	     rightEntry = entries[i];
    }
    if ( ( rightEntry == NULL ) && ( theRightSiblingName != "" ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkAndGetSiblings",
	    " Requested right sibling >> %s << can not be found in the nodekitCatalog", theRightSiblingName.getString() );
#endif
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used as a check when adding a new entry to a catalog.
//    Called if the new node is to be a list of other nodes.
//    Checks that the given type is OK as a list.
//
// Use: private

SbBool
SoV1NodekitCatalog::checkCanTypesBeList( const SoType &theType,
					const SoType &theDefaultType )
//
////////////////////////////////////////////////////////////////////////
{
    // CHECK IF IT'S A GROUP OR SEPARATOR
    if ( !(theType == SoGroup::getClassTypeId()     ||
	   theType == SoSeparator::getClassTypeId() ||
	   theType == SoSwitch::getClassTypeId() )) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkCanTypesBeList",
	    " requested node type can not be a list since it is neither an SoGroup, an  SoSeparator, or an SoSwitch");
#endif
	return FALSE;
    }
    if ( !(theDefaultType == SoGroup::getClassTypeId()     ||
	   theDefaultType == SoSeparator::getClassTypeId() ||
	   theDefaultType == SoSwitch::getClassTypeId() )) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::checkCanTypesBeList",
	    " requested node type can not be a list since it is neither an SoGroup, an  SoSeparator, or an SoSwitch");
#endif
	return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For adding a new entry into the nodekitCatalog
//
// Use: internal

SbBool
SoV1NodekitCatalog::addEntry( const SbName &theName, 
			    const SoType &theType,
			    const SoType &theDefaultType,
			    const SbName &theParentName, 
			    const SbName &theRightSiblingName, 
			    SbBool theListPart, 
			    const SoType &theListItemType,
			    SbBool thePublicPart )
//
////////////////////////////////////////////////////////////////////////
{
    SoV1NodekitCatalogEntry *parentEntry, *rightEntry, *leftEntry;
    SoV1NodekitCatalogEntry *newEntry;
    SoV1NodekitCatalogEntry **newArray;

    // CHECK IF THE NEW ENTRY IS OK

    if ( !checkNewName( theName ) )
	return FALSE;

    if ( !checkNewTypes( theType, theDefaultType ) ) {
#ifdef DEBUG
	SoDebugError::post("SoV1NodekitCatalogEntry::addEntry",
	    "error creating catalog entry %s", theName.getString());
#endif
	return FALSE;
    }

    if ( !checkAndGetParent( theName, theParentName, parentEntry ) )
	return FALSE;

    if ( !checkAndGetSiblings( theParentName, theRightSiblingName, 
			       leftEntry, rightEntry ) )
	return FALSE;

    if ( theListPart && !checkCanTypesBeList( theType, theDefaultType ) )
	return FALSE;

    // IF ALL TESTS WERE PASSED...

    // expand the list by one slot
    newArray = new ( SoV1NodekitCatalogEntry *[numEntries + 1]);
    if ( entries != NULL ) {
	for (int i = 0; i < numEntries; i++ )
	    newArray[i] = entries[i];
	delete entries;
    }
    entries = newArray;
    numEntries++;	

    // make a list containing only the given list item type.
    SbPList *listItemTypeList = new SbPList( 0 );
    SoType  *typePtr = new SoType;
    *typePtr = theListItemType;
    listItemTypeList->append( (void *) typePtr );

    // create the new entry
    newEntry = new SoV1NodekitCatalogEntry( theName, theType, theDefaultType,
	   theParentName, theRightSiblingName,theListPart,*listItemTypeList,
	   thePublicPart);
    // enter the new entry in the array
    entries[numEntries - 1] = newEntry;

    // add the new name to the quick-reference part name dictionary
    partNameDict.enter( (unsigned long) theName.getString(),
			(void *) (numEntries - 1));

    // parent is no longer a leaf node in the nodekit structure
    if ( parentEntry != NULL )
        parentEntry->setLeaf( FALSE );

    // stitch up sibling names.
    if ( leftEntry != NULL )
	leftEntry->setRightSiblingName( theName );
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks for part with the given name within the template.
//    Recursively Checks 'partNumber' only.
//
//    See SoV1NodekitCatalogEntry::recursiveSearch for more info...
//
// Use: public
SbBool
SoV1NodekitCatalog::recursiveSearch( int partNumber,
				     const SbName    &nameToFind, 
				     SbPList   *typesChecked ) const
//
////////////////////////////////////////////////////////////////////////
{
    // just call the recursive search method on the given entry...
    return( entries[partNumber]->recursiveSearch( nameToFind, typesChecked ));
}
