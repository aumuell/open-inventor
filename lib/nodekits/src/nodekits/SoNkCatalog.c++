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
 |      SoNodekitCatalog
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodekits/SoNodekitCatalog.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

// Static variables for the nodekit catalog class...

const SbName  *SoNodekitCatalog::emptyName = NULL;
const SoTypeList *SoNodekitCatalog::emptyList = NULL;
SoType  *SoNodekitCatalog::badType   = NULL;

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoNodekitCatalogEntry
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoNodekitCatalogEntry::SoNodekitCatalogEntry( const SbName &theName, 
			      SoType  theType , SoType theDefaultType,
			      SbBool   theNullByDefault,
			const SbName  &theParentName, 
			const SbName  &theRightSiblingName, SbBool theListPart, 
			      SoType  theListContainerType,
			const SoTypeList &theListItemTypes,
			      SbBool thePublicPart )
//
////////////////////////////////////////////////////////////////////////
{
    name                   = theName;
    type                   = theType;
    defaultType            = theDefaultType;
    nullByDefault   	   = theNullByDefault;
    leafPart               = TRUE;    // everything is a leaf 'til given a child
    parentName             = theParentName;
    rightSiblingName       = theRightSiblingName;
    listPart               = theListPart;
    listContainerType      = theListContainerType;
    listItemTypes.copy( theListItemTypes );
    publicPart             = thePublicPart;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog
//
// Use: private

SoNodekitCatalogEntry *
SoNodekitCatalogEntry::clone() const
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

SoNodekitCatalogEntry *
SoNodekitCatalogEntry::clone( SoType newType, 
			      SoType newDefaultType ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoNodekitCatalogEntry *theClone;

    theClone = new SoNodekitCatalogEntry( name, newType, newDefaultType,
	 nullByDefault, parentName, rightSiblingName, listPart, 
	 listContainerType, listItemTypes, publicPart );
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
SoNodekitCatalogEntry::addListItemType( SoType typeToAdd )
//
////////////////////////////////////////////////////////////////////////
{
    listItemTypes.append( typeToAdd );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks for the given part.
//    Checks this part, then recursively checks all entries in the 
//    catalog of this part (it gets its own catalog by looking
//    at the catalog for the 'dummy' for this type, which is accessed
//    through: type.getInstance()->getNodekitCatalog()
//    or, if type is an abstract type, 
//    uses defaultType.getInstance()->getNodekitCatalog()
//
// Use: public

SbBool
SoNodekitCatalogEntry::recursiveSearch( const SbName    &nameToFind, 
					  SoTypeList   *typesChecked ) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoNodekitCatalog *subCat;

    // is this the part of my dreams?
    if ( name == nameToFind )
	return TRUE;

    // make sure the part isn't a list
    if ( listPart == TRUE )
	return FALSE;
    // make sure the part is subclassed off of SoBaseKit
    if ( !type.isDerivedFrom( SoBaseKit::getClassTypeId() ))
	return FALSE;

    // avoid an infinite search loop by seeing if this type has already been
    // checked...
    if ( typesChecked->find( type ) != -1 )
	return FALSE;

    // if it's still ok, then search within the catalog of this part
    // first check each name:
    const SoBaseKit *inst = (const SoBaseKit *) type.createInstance();
    if ( inst == NULL )
	inst = (const SoBaseKit *) defaultType.createInstance();
    if ( inst == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalogEntry::recursiveSearch",
	"part type and defaultType are both abstract classes");
	abort();
#endif
    }

    subCat = inst->getNodekitCatalog();
    inst->ref();
    inst->unref();

    int i;
    for( i = 0; i < subCat->getNumEntries(); i++ ) {
	if ( subCat->getName( i ) == nameToFind ) 
		return TRUE;
    }
    // at these point, we've checked all the names in this class, so 
    // we can add it to typesChecked
    typesChecked->append( type );

    // then, recursively check each part
    for( i = 0; i < subCat->getNumEntries(); i++ ) {
	if ( subCat->recursiveSearch( i, nameToFind, typesChecked ) )
		return TRUE;
    }

    return FALSE;  // couldn't find it ANYwhere!
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Print contents for debugging purposes
//
// Use: internal

void 
SoNodekitCatalogEntry::printCheck() const
//
////////////////////////////////////////////////////////////////////////
{
    fprintf( stdout,"    name = %s, type = %d, defaultType = %d\n",
	    name.getString(), type, defaultType );
    fprintf( stdout,"    nullByDefault = %d\n", nullByDefault);
    fprintf( stdout,"    parentName = %s\n", parentName.getString() );
    fprintf( stdout,"    sibling = %s, listPart = %d\n",
	    rightSiblingName.getString(), listPart );
    if ( listPart ) {
	fprintf( stdout, "listItemTypes = " );
	for ( int i = 0; i < listItemTypes.getLength(); i++ ) {
	    fprintf( stdout,"  %d  ", listItemTypes[i].getName().getString() );
	}
	fprintf( stdout, "\n" );
    }
    fprintf( stdout,"    publicPart = %d\n", publicPart );
}

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SoNodekitCatalog
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: internal

SoNodekitCatalog::SoNodekitCatalog()
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

SoNodekitCatalog::~SoNodekitCatalog()
//
////////////////////////////////////////////////////////////////////////
{
    // delete all the members of the entries array
    for ( int i = 0; i < numEntries; i++ )
	delete entries[i];

    // delete the entries array itself
    if (entries != NULL)
	delete [] entries;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints out contents of the catalog for debuggin purposes
//
// Use: internal

void
SoNodekitCatalog::printCheck() const
//
////////////////////////////////////////////////////////////////////////
{
    fprintf( stdout, "catalog printout: number of entries = %d\n", numEntries );
    for( int i = 0; i < numEntries; i++ ) {
	fprintf( stdout, "#%d\n", i );
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
SoNodekitCatalog::getPartNumber( const SbName &theName ) const
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
	return SO_CATALOG_NAME_NOT_FOUND;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the name of an entry given its partNumber.
//
// Use: internal

const SbName &
SoNodekitCatalog::getName( int thePartNumber ) const
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

SoType
SoNodekitCatalog::getType( int thePartNumber ) const
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

SoType
SoNodekitCatalog::getType( const SbName &theName ) const
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

SoType
SoNodekitCatalog::getDefaultType( int thePartNumber ) const
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

SoType
SoNodekitCatalog::getDefaultType( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getDefaultType( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'nullByDefault' of an entry.
//
// Use: internal

SbBool
SoNodekitCatalog::isNullByDefault( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the value for this entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->isNullByDefault();
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
SoNodekitCatalog::isNullByDefault( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( isNullByDefault( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding 'leafPart' of an entry.
//
// Use: internal

SbBool
SoNodekitCatalog::isLeaf( int thePartNumber ) const
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
SoNodekitCatalog::isLeaf( const SbName &theName ) const
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
SoNodekitCatalog::getParentName( int thePartNumber ) const
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
SoNodekitCatalog::getParentName( const SbName &theName ) const
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
SoNodekitCatalog::getParentPartNumber( int thePartNumber ) const
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
SoNodekitCatalog::getParentPartNumber( const SbName &theName ) const
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
SoNodekitCatalog::getRightSiblingName( int thePartNumber ) const
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
SoNodekitCatalog::getRightSiblingName( const SbName &theName ) const
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
SoNodekitCatalog::getRightSiblingPartNumber( int thePartNumber ) const
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
SoNodekitCatalog::getRightSiblingPartNumber( const SbName &theName ) const
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
SoNodekitCatalog::isList( int thePartNumber ) const
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
SoNodekitCatalog::isList( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( isList( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the type of the container of a list entry.
//
// Use: internal

SoType
SoNodekitCatalog::getListContainerType( int thePartNumber ) const
//
////////////////////////////////////////////////////////////////////////
{
    // return the defaultType of the entry, if you can find it.
    if ( thePartNumber >= 0 && thePartNumber < numEntries )
	return entries[thePartNumber]->getListContainerType();
    else
	return *badType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the type of the container of a list entry.
//
// Use: internal

SoType
SoNodekitCatalog::getListContainerType( const SbName &theName ) const
//
////////////////////////////////////////////////////////////////////////
{
    return( getListContainerType( getPartNumber( theName ) ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For finding the list item type of an entry.
//
// Use: internal

const SoTypeList &
SoNodekitCatalog::getListItemTypes( int thePartNumber ) const
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

const SoTypeList &
SoNodekitCatalog::getListItemTypes( const SbName &theName ) const
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
SoNodekitCatalog::isPublic( int thePartNumber ) const
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
SoNodekitCatalog::isPublic( const SbName &theName ) const
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
SoNodekitCatalog::addListItemType( int thePartNumber,
					SoType typeToAdd )
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
SoNodekitCatalog::addListItemType( const SbName &theName,
					SoType typeToAdd )
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
// For example, in SoShapeKit, the part "shape" has type SoShape and
// default type SoSphere.  Any shape node is acceptable, but be default a 
// sphere will be built.
// Well, when creating the SoVertexShapeKit class, a call of:
//    narrowTypes( "shape", SoVertexShape::getClassTypeId(),
//                          SoFaceSet::getClassTypeId())
// might be used.  This would reflect the fact that:
// Only vertext shapes may be put in the "shape"  part, not just any shape.
// And also, by default, a faceSet will be built, not a sphere.
//
// Use: public

void
SoNodekitCatalog::narrowTypes( const SbName &theName,
			SoType newType, SoType newDefaultType )
//
////////////////////////////////////////////////////////////////////////
{
    int thePartNumber = getPartNumber( theName );

    if ( thePartNumber < 0 || thePartNumber >= numEntries )
	return;

    SoNodekitCatalogEntry *theEntry = entries[thePartNumber];

    // Checks that [1] newDefaultType is not an abstract type, and 
    //             [2] newDefaultType is a subclass of newType
    if ( !checkNewTypes( newType, newDefaultType ) )
	return;

    SoType oldType        = theEntry->getType();
    SoType oldDefaultType = theEntry->getDefaultType();

    // Make sure that the new types is derived from the old type.
    // Parts in derived classes must be subclasses of the types they
    // belong to when cast to the parent classes.
    if ( !newType.isDerivedFrom( oldType ) ) {
#ifdef DEBUG
	const char *newName = newType.getName().getString();
	const char *oldName = oldType.getName().getString();
	SoDebugError::post("SoNodekitCatalog::narrowTypes",
		   "The newType %s is not a subclass of the oldType %s. "
		   "Cannot narrow the  type from %s to %s",
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
//
// For changing whether a part is created by default (during the constructor)
// by nodekits that use this catalog.
//
// Use: EXTENDER public

void
SoNodekitCatalog::setNullByDefault( const SbName &theName, 
				    SbBool newNullByDefault )
//
////////////////////////////////////////////////////////////////////////
{
    int thePartNumber = getPartNumber( theName );

    if ( thePartNumber < 0 || thePartNumber >= numEntries )
	return;

    entries[thePartNumber]->setNullByDefault( newNullByDefault );
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a new copy of this catalog
//
// Use: public

SoNodekitCatalog *
SoNodekitCatalog::clone( SoType typeOfThis ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoNodekitCatalog      *theClone;

    theClone = new SoNodekitCatalog;
    theClone->numEntries = numEntries;
    if (numEntries == 0)
	theClone->entries = NULL;
    else {
	theClone->entries = new ( SoNodekitCatalogEntry *[numEntries]);
	for (int i = 0; i < numEntries; i++) {
	    if ( i == SO_CATALOG_THIS_PART_NUM )
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
SoNodekitCatalog::checkName( const SbName &theName )  // proposed name
//
////////////////////////////////////////////////////////////////////////
{
    // CHECK IF IT'S NULL
    if ( theName.getString() == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkName",
			    "given name is NULL" );
#endif
  	return FALSE;
    }

    // CHECK IF IT'S EMPTY
    if ( theName == "" ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkName",
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
SoNodekitCatalog::checkNewName( const SbName &theName )  // proposed name
//
////////////////////////////////////////////////////////////////////////
{
    if ( !checkName( theName ) )
	return FALSE;

    // CHECK IF IT'S UNIQUE FOR THIS CATALOG
    if ( getPartNumber( theName ) == SO_CATALOG_NAME_NOT_FOUND ) {
	return TRUE;
    }
    else {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkNewName",
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
SoNodekitCatalog::checkNewTypes( SoType theType,  // proposed type
				 SoType theDefaultType ) // and default
//
////////////////////////////////////////////////////////////////////////
{
    if ( !theDefaultType.canCreateInstance()) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkNewTypes",
	"the defaultType %s is an abstract class, and cannot be part of a nodekits structure",
				theDefaultType.getName().getString() );
#endif
  	return FALSE;
    }

    if ( !theDefaultType.isDerivedFrom( theType ) ) {
#ifdef DEBUG
	const char *defName = theDefaultType.getName().getString();
	SoDebugError::post("SoNodekitCatalog::checkNewTypes",
			   "the defaultType %s is not a subclass of the type "
			   "%s. It can not be part of a nodekits structure",
			   defName, theType.getName().getString());
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
SoNodekitCatalog::checkAndGetParent( 
	const SbName                &theName,          // the child's name
	const SbName                &theParentName,    // proposed parent name
	SoNodekitCatalogEntry *& parentEntry ) // corresponding entry filled 
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
    if ( parentPartNumber == SO_CATALOG_NAME_NOT_FOUND ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkAndGetParent",
        "requested parent >> %s << is not in this catalog",
	    			theParentName.getString() );
#endif
	return FALSE;
    }
    parentEntry = entries[ parentPartNumber ];

    // CHECK THE NODE TYPE OF THE PARENT

    // [1] Unless it's "this", it must be a subclass of SoGroup, 
    //     or you can't add children to it.
    if ( parentPartNumber !=  SO_CATALOG_THIS_PART_NUM &&
	 !parentEntry->getType().isDerivedFrom( SoGroup::getClassTypeId() )) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkAndGetParent",
        "requested parent >> %s << is a node that is not subclassed from SoGroup, so it can\'t have children",
	    			theParentName.getString() );
#endif
	return FALSE;
    }
    // [2] If they didn't call 'initClass', then 'theParentName' will == 'this',
    //     but parentEntry->getType() will not yet be something derived from
    //     SoBaseKit. Check for this.
    if ( parentPartNumber ==  SO_CATALOG_THIS_PART_NUM &&
	 !parentEntry->getType().isDerivedFrom( SoBaseKit::getClassTypeId() )) {
#ifdef DEBUG
	SoDebugError::post( "SoNodekitCatalog::checkAndGetParent",
	 "  It looks like you forgot to call initClass for one of your nodekit classes! Expect a core dump!");
#endif
    }
    // [3] Unless it is 'this', the parent can NOT be subclass of SoBaseKit.
    //     This is because you can only add child nodes to an nodekit through 
    //     its own class's nodekitCatalog
    if ( parentPartNumber !=  SO_CATALOG_THIS_PART_NUM &&
	 parentEntry->getType().isDerivedFrom( SoBaseKit::getClassTypeId()) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkAndGetParent",
        " Requested parent >> %s << is a node that is subclassed from SoBaseKit, so it can\'t children except through its own class\'s nodekitCatalog",
	    			theParentName.getString() );
#endif
	return FALSE;
    }

    // MAKE SURE THAT THE NODE HAS NOT BEEN DESIGNATED AS A LIST
    if ( parentEntry->isList() == TRUE ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkAndGetParent",
        " Requested parent >> %s << is a node that has been designated as a list.  \n It can not have explicit children in the catalog although nodes \n can be added to the list for a given instance by using the special list editting \n methods on the nodekit node.",
	    			theParentName.getString() );
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
SoNodekitCatalog::checkAndGetSiblings( 
	const SbName            &theParentName,       // parent name
	const SbName            &theRightSiblingName, // proposed sibling name
	SoNodekitCatalogEntry *& leftEntry,      // new left (to be filled in)
	SoNodekitCatalogEntry *& rightEntry )    // new right (to be filled )
//
////////////////////////////////////////////////////////////////////////
{
    leftEntry = NULL;
    rightEntry = NULL;

    for ( int i = 0; i < numEntries; i++ ) {
	if ( entries[i]->getParentName() == theParentName ) {
	    // is it the left sibling?
	    if ( entries[i]->getRightSiblingName() == theRightSiblingName )
		 leftEntry = entries[i];
	    // is it the right sibling?
	    else if ( theRightSiblingName == entries[i]->getName() )
	         rightEntry = entries[i];
	}
    }
    if ( ( rightEntry == NULL ) && ( theRightSiblingName != "" ) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkAndGetSiblings",
	"Requested right sibling >> %s << can not be found in the nodekitCatalog",
				theRightSiblingName.getString() );
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
SoNodekitCatalog::checkCanTypesBeList(   SoType theType,
				         SoType theDefaultType,
					 SoType theListContainerType )
//
////////////////////////////////////////////////////////////////////////
{
    // CHECK IF IT'S A GROUP OR SEPARATOR
    if ( !theType.isDerivedFrom( SoNodeKitListPart::getClassTypeId() ) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkCanTypesBeList",
        "requested node type can not be a list. It is not derived from SoNodeKitListPart");
#endif
	return FALSE;
    }
    if ( !theDefaultType.isDerivedFrom( theType ) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkCanTypesBeList",
        "requested default type is not derived from requested type.");
#endif
	return FALSE;
    }
    if ( !theListContainerType.isDerivedFrom( SoGroup::getClassTypeId() ) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::checkCanTypesBeList",
        " requested list container node type can not be used. It is not derived from SoGroup");
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
SoNodekitCatalog::addEntry( const SbName &theName, 
			          SoType theType,
			          SoType theDefaultType,
				  SbBool  theNullByDefault,
			    const SbName &theParentName, 
			    const SbName &theRightSiblingName, 
			          SbBool theListPart, 
			          SoType theListContainerType,
			          SoType theListItemType,
			    SbBool thePublicPart )
//
////////////////////////////////////////////////////////////////////////
{
    SoNodekitCatalogEntry *parentEntry, *rightEntry, *leftEntry;
    SoNodekitCatalogEntry *newEntry;
    SoNodekitCatalogEntry **newArray;

    // CHECK IF THE NEW ENTRY IS OK

    if ( !checkNewName( theName ) )
	return FALSE;

    if ( !checkNewTypes( theType, theDefaultType ) ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::addEntry",
		" Error creating catalog entry %s", theName.getString());
#endif
	return FALSE;
    }

    if ( !checkAndGetParent( theName, theParentName, parentEntry ) )
	return FALSE;

    if ( !checkAndGetSiblings( theParentName, theRightSiblingName, 
			       leftEntry, rightEntry ) )
	return FALSE;

    if ( theListPart && !checkCanTypesBeList( theType, theDefaultType, 
					      theListContainerType ) )
	return FALSE;

    if ( numEntries == SO_CATALOG_THIS_PART_NUM  && theName != "this" ) {
#ifdef DEBUG
	SoDebugError::post("SoNodekitCatalog::addEntry",
		" Entry number %d must be named \"this\" ",
		SO_CATALOG_THIS_PART_NUM );
#endif
	return FALSE;
    }

    // IF ALL TESTS WERE PASSED...

    // expand the list by one slot
    newArray = new ( SoNodekitCatalogEntry *[numEntries + 1]);
    if ( entries != NULL ) {
	for (int i = 0; i < numEntries; i++ )
	    newArray[i] = entries[i];
	delete [] entries;
    }
    entries = newArray;
    numEntries++;	

    // make a list containing only the given list item type.
    SoTypeList listItemTypeList(0);
    listItemTypeList.append( theListItemType );

    // create the new entry
    newEntry = new SoNodekitCatalogEntry( theName, theType, theDefaultType,
	    theNullByDefault, theParentName, theRightSiblingName,
	    theListPart,theListContainerType, listItemTypeList, thePublicPart);
    // enter the new entry in the array
    entries[numEntries - 1] = newEntry;

    // add the new name to the quick-reference part name dictionary
    partNameDict.enter( (unsigned long) theName.getString(), 
			(void *) (numEntries - 1));

    // parent is no longer a leaf node in the nodekit structure
    if ( parentEntry != NULL ) {
        parentEntry->setLeaf( FALSE );
        parentEntry->setPublic( FALSE );
    }

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
//    See SoNodekitCatalogEntry::recursiveSearch for more info...
//
// Use: public
SbBool
SoNodekitCatalog::recursiveSearch( int partNumber,
				     const SbName    &nameToFind, 
				     SoTypeList   *typesChecked ) const
//
////////////////////////////////////////////////////////////////////////
{
    // just call the recursive search method on the given entry...
    return( entries[partNumber]->recursiveSearch( nameToFind, typesChecked ));
}
