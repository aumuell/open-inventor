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
 |      Defines the SoNodekitCatalog class. This class is used to 
 |      describe the subgraphs that make up different classes of 
 |      nodekit.	
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_NODEKIT_CATALOG
#define  _SO_NODEKIT_CATALOG

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbString.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoType.h>

#define SO_CATALOG_NAME_NOT_FOUND -1
#define SO_CATALOG_THIS_PART_NUM   0

////////////////////////////////////////////////////////////////////
//
//    Class: SoNodekitCatalogEntry
//
//    an internal class that should be accessed only by an SoNodekitCatalog
//
//
////////////////////////////////////////////////////////////////////
SoINTERNAL class SoNodekitCatalogEntry {
    private:
	SbName     name;		// Reference name of this entry 
	SoType     type;		// Type of node allowable for setting
					// this entry. 'type' may be an 
					// abstract node type
	SoType     defaultType;		// Type of node to create if the nodekit
					// is asked to build this part itself.
					// May not be an abstract node type.
	SbBool     nullByDefault;       // If FALSE, then this part is 
					// created within the constructor.
					// Otherwise, it isn't.
	SbBool     leafPart;            // Is this entry a leaf node in the 
					// nodekit's structure?
	SbName     parentName;		// Reference name of this entry's parent
	SbName     rightSiblingName;    // Name of right sibling of the entry
	SbBool     listPart;            // Is this entry a list of nodes?
	SoType     listContainerType;   // If so, what kind of container?
	SoTypeList listItemTypes;       // If so, what types of nodes may
					// it contain?
	SbBool     publicPart;          // Can a user be given a pointer to
					// this part?
    SoINTERNAL public:
	// constructor
	SoNodekitCatalogEntry(const SbName &theName, 
		    SoType theType, SoType theDefaultType, 
		    SbBool theNullByDefault,
		    const SbName &theParentName, const SbName &theRightSiblingName, 
		    SbBool theListPart, SoType theListContainerType,
		    const SoTypeList &theListItemTypes,
		    SbBool thePublicPart);

	// clone exactly, or make a clone with a given type...
	SoNodekitCatalogEntry *clone() const;           
	SoNodekitCatalogEntry *clone( SoType newType,
				      SoType newDefaultType ) const; 

	// inquiry functions:
	const SbName  &getName() const { return name; };
	      SoType  getType() const { return type; };
	      SoType  getDefaultType() const { return defaultType; };
	      SbBool  isNullByDefault() const { return nullByDefault; };
	      SbBool  isLeaf() const { return leafPart; };
	const SbName  &getParentName() const { return parentName; };
	const SbName  &getRightSiblingName() const { return rightSiblingName; };
	      SbBool  isList() const { return listPart; };
	      SoType  getListContainerType() const {return listContainerType;};
	const SoTypeList &getListItemTypes() const { return listItemTypes; };
	      SbBool  isPublic() const { return publicPart; };

	// For adding to the set of node types allowed beneath a list...
	void addListItemType( SoType typeToAdd );

	void setNullByDefault( SbBool newNullByDefault ) 
		{ nullByDefault = newNullByDefault; }
	// these should only by used by catalogs when an entry
	// is given a new child or left sibling...
	void setPublic( SbBool newPublic ) { publicPart = newPublic; }
	void setLeaf( SbBool newLeafPart ) { leafPart = newLeafPart; }
	void setRightSiblingName( const SbName &newN ) { rightSiblingName = newN; }
	// This should only by used by catalogs when an entry is changing
	// type and/or defaultType
	void setTypes( SoType newType, SoType newDefaultType )
			    { type = newType; defaultType = newDefaultType; }

	// recursively search this entry for 'name to find' in the 
	// templates of this entry and its descendants
	SbBool recursiveSearch( const SbName &nameToFind, 
				SoTypeList *typesChecked) const;

	void printCheck() const; // prints the contents of this entry
};

////////////////////////////////////////////////////////////////////
//    Class: SoNodekitCatalog
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoNkitCat
class SoNodekitCatalog {

  public:

    // initializes static variables. 
    static void initClass();

    // How many entries in this catalog?
    int        getNumEntries() const { return numEntries; };

    // inquiry routines, to find out about entries in the catalog,
    // Questions may be asked based on name or partNumber.
    // C-api: name=getPartNum
	  int        getPartNumber(      const SbName &theName ) const;
    const SbName     &getName(		       int    thePartNumber ) const;
	  SoType     getType(		       int    thePartNumber ) const;
    // C-api: name=getTypeByName
	  SoType     getType(		 const SbName &theName ) const;
    // C-api: name=getDfltType
	  SoType     getDefaultType(	       int    thePartNumber ) const;
    // C-api: name=getDfltTypeByName
	  SoType     getDefaultType(	 const SbName &theName ) const;
	  SbBool     isNullByDefault(	       int    thePartNumber ) const;
    // C-api: name=isNullByDefaultByName
	  SbBool     isNullByDefault(		 const SbName &theName ) const;
	  SbBool     isLeaf(		       int    thePartNumber ) const;
    // C-api: name=isLeafByName
	  SbBool     isLeaf(		 const SbName &theName ) const;
    // C-api: name=getParName
    const SbName     &getParentName(	       int    thePartNumber ) const;
    // C-api: name=getParNameByName
    const SbName     &getParentName(	 const SbName &theName ) const;
    // C-api: name=getParPartNum
	  int        getParentPartNumber(      int    thePartNumber ) const;
    // C-api: name=getParPartNumByName
	  int        getParentPartNumber(const SbName &theName ) const;
    // C-api: name=getRtSibName
    const SbName     &getRightSiblingName(      int    thePartNumber ) const;
    // C-api: name=getRtSibNameByName
    const SbName     &getRightSiblingName(const SbName &theName ) const;
    // C-api: name=getRtSibPartNum
	  int        getRightSiblingPartNumber(      int thePartNumber ) const;
    // C-api: name=getRtSibPartNumByName
	  int        getRightSiblingPartNumber(const SbName &theName ) const;
	  SbBool     isList(		       int    thePartNumber ) const;
    // C-api: name=isListByName
	  SbBool     isList(		 const SbName &theName ) const;
    // C-api: name=getLstCntnrType
	  SoType     getListContainerType(       int    thePartNumber ) const;
    // C-api: name=getLstCntnrTypeByName
	  SoType     getListContainerType( const SbName &theName ) const;
    const SoTypeList    &getListItemTypes(       int    thePartNumber ) const;
    // C-api: name=getListItemTypesByName
    const SoTypeList    &getListItemTypes( const SbName &theName ) const;
	  SbBool     isPublic(		       int    thePartNumber ) const;
    // C-api: name=isPubByName
	  SbBool     isPublic(		 const SbName &theName ) const;

  SoEXTENDER public:
    // Catalogs are only constructed, destructed, cloned or added to  
    // by subclasses of SoBaseKit.
    // This should be accomplished using the macros provided in SoSubKit.h

    // Constructor
    SoNodekitCatalog();

    // Destructor
    ~SoNodekitCatalog();

    // Make a new identical copy, but you must give the node type for 'this' 
    // (where 'this' is the top level node in the template )
    SoNodekitCatalog *clone( SoType typeOfThis ) const;

    // Adding entries
    SbBool addEntry(const SbName &theName, 
		          SoType theType, SoType theDefaultType,
		          SbBool  theNullByDefault,
		    const SbName &theParentName, 
	            const SbName &theRightSiblingName, SbBool theListPart, 
	                  SoType theListContainerType,
			  SoType theListItemType,
			  SbBool thePublicPart);

    // For adding to the set of node types allowed beneath a list...
    void addListItemType(int thePartNumber, SoType typeToAdd);
    // C-api: name=addListItemTypeByName
    void addListItemType(const SbName &theName,   SoType typeToAdd);

    // For changing the type and defaultType of an entry.
    // The new types must be subclasses of the types already existing in the
    // entry.  
    // For example, in SoShapeKit, the part "shape" has type SoShape and
    // default type SoSphere.  Any shape node is acceptable, but be default a 
    // sphere will be built.
    // Well, when creating the SoVertexShapeKit class, a call of:
    //    narrowTypes( "shape", SoVertexShape::getClassTypeId(), 
    //			        SoFaceSet::getClassTypeId())
    // might be used.  This would reflect the fact that:
    // Only vertext shapes may be put in the "shape"  part, not just any shape.
    // And also, by default, a faceSet will be built, not a sphere.
    void narrowTypes( const SbName &theName, SoType newType, 
		      SoType newDefaultType );

    void setNullByDefault( const SbName &theName, SbBool newNullByDefault );

  SoINTERNAL public:
    // used by SoNodekitParts to search through catalogs.
    // recursively search a given part for 'name to find' in the 
    // templates of that entry and its descendants
    SbBool recursiveSearch( int partNumber, const SbName &nameToFind, 
			    SoTypeList *typesChecked) const;

    void printCheck() const; // prints the contents of this catalog

  private:

    static const SbName  *emptyName;
    static const SoTypeList *emptyList;
    static SoType  *badType;

    int 		  numEntries;   // how many entries?
    SoNodekitCatalogEntry **entries;    // the array of entries
    SbDict                partNameDict; // For fast lookup of part numbers

    // Used for testing various aspects of new entries into a catalog
    SbBool    checkName( const SbName &theName );
    SbBool    checkNewName( const SbName &theName );
    SbBool    checkNewTypes( SoType theType, 
			     SoType theDefaultType );
    SbBool    checkAndGetParent(    const SbName &theName, 
    				    const SbName &theParentName, 
				    SoNodekitCatalogEntry *& parentEntry );
    SbBool    checkAndGetSiblings(  const SbName &theParentName, 
				    const SbName &theRightSiblingName, 
				    SoNodekitCatalogEntry *& leftEntry,
				    SoNodekitCatalogEntry *& rightEntry );
    SbBool    checkCanTypesBeList(  SoType theType,
    				    SoType theDefaultType,
				    SoType theListContainerType );

};

#endif  /* _SO_NODEKIT_CATALOG */
