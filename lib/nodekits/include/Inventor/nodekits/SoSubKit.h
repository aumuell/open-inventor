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
 |	This file defines some macros that implement things common to
 |	many subclasses of SoBaseKit. They may be used to make SoBaseKit
 |	subclassing easier. In all of the macros:
 |      "className" refers to the name of the baseKit subclass.
 |      "partName"  refers to the name of a part in the nodekit catalog.
 |      "partClassName" refers to the class to which a part in the 
 |                      the nodekit catalog belongs. 
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of any nodekit:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure nodekitClassName.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |
 |   Defined macros:
 |
 |	Within class header:
 |		SO_KIT_HEADER(className)
 |		SO_KIT_CATALOG_ENTRY_HEADER(partName)
 |
 |	Within class source:
 |	    At file scope:
 |		SO_KIT_SOURCE(className)
 |
 |	    Class initialization:
 |		SO_KIT_INIT_CLASS(className,parentClass,parentPrintClass)
 |
 |	    Constructor initialization:
 |		SO_KIT_CONSTRUCTOR(className)
 |		SO_KIT_ADD_CATALOG_ENTRY(partName, partClassName, nullByDefault, |                     parentName, rightName, isPublicPart )
 |		SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(partName, partClassName, 
 |                     defaultPartClassName, nullByDefault, 
 |                     parentName,  rightName, isPublicPart )
 |              SO_KIT_ADD_CATALOG_LIST_ENTRY(partName, listContainerClassName, 
 |                     nullByDefault, parentName, rightName, 
 |                     listItemClassName, isPublicPart )
 |              SO_KIT_ADD_LIST_ITEM_TYPE(partName, newListItemClassName )
 |              SO_KIT_CHANGE_ENTRY_TYPE(partName, newPartClassName, 
 |                     newDefaultPartClassName)
 |              SO_KIT_CHANGE_NULL_BY_DEFAULT(partName, newNullByDefault)
 |
 |              SO_KIT_INIT_INSTANCE()
 |
 |		SO_KIT_ADD_FIELD(fieldMember,(defaultValue))
 |		SO_KIT_DEFINE_ENUM_VALUE(enumType,enumValue)
 |		SO_KIT_IS_FIRST_INSTANCE()	// a boolean value
 |		SO_KIT_SET_SF_ENUM_TYPE(fieldName,enumType)
 |		SO_KIT_SET_MF_ENUM_TYPE(fieldName,enumType)
 |
 |   Author(s)		: Paul Isaacs, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// There is a cycle of include files between SoBaseKit.h and SoSubKit.h
// SoBaseKit.h must be included before _SO_SUB_KIT_ is defined.  Otherwise,
// SoBaseKit.h will not compile.
#include <Inventor/nodekits/SoBaseKit.h>

#ifndef  _SO_SUB_KIT_
#define  _SO_SUB_KIT_

// C-api: end

#include <Inventor/nodes/SoNode.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodekits/SoNodekitCatalog.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for a SoBaseKit
// subclass:
//

////////////////////////////////////////////
//
// This defines a catalog describing the arrangement 
// of the subgraph for instances of this class
//

#define SO__KIT_CATALOG_HEADER(className)				      \
  private:								      \
    static SoNodekitCatalog *nodekitCatalog; /* design of this class */       \
    static const SoNodekitCatalog **parentNodekitCatalogPtr;/* parent design */\
  public:								      \
    static const SoNodekitCatalog *getClassNodekitCatalog()		      \
	{ return nodekitCatalog; } 					      \
    /* Returns an SoNodekitCatalog for the node */                            \
    virtual const SoNodekitCatalog *getNodekitCatalog() const;		      \
  protected:								      \
    static const SoNodekitCatalog **getClassNodekitCatalogPtr()		      \
	{ return (const SoNodekitCatalog **)&nodekitCatalog; }

#define SO_KIT_HEADER(className)	                                      \
    SO_NODE_HEADER(className);	                                              \
    SO__KIT_CATALOG_HEADER(className)


////////////////////////////////////////////
//
// This defines an SoSFNode field for the catalog part given.
// The field will always be a (protected) node-pointer field.
// It will keep track of which (hidden) child is which part.
//

#define SO_KIT_CATALOG_ENTRY_HEADER(partName)			              \
  protected:								      \
    SoSFNode partName


/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a node subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO__KIT_CATALOG_HEADER.
//

#define SO__KIT_CATALOG_VARS(className)				               \
SoNodekitCatalog   *className::nodekitCatalog = NULL;                          \
const SoNodekitCatalog   **className::parentNodekitCatalogPtr = NULL

////////////////////////////////////////////
//
// This implements the methods defined in SO__KIT_CATALOG_HEADER.
//

#define SO__KIT_CATALOG_METHODS(className)			              \
const SoNodekitCatalog *						      \
className::getNodekitCatalog() const					      \
{									      \
    return nodekitCatalog;					              \
}

////////////////////////////////////////////
//
// These include all the definitions required
// at file scope
//

#define SO_KIT_SOURCE(className)	                                      \
    SO_NODE_SOURCE(className);	                                              \
    SO__KIT_CATALOG_VARS(className);	                                      \
    SO__KIT_CATALOG_METHODS(className)

////////////////////////////////////////////
//
// Internal initialization macros
// 
#define SO__KIT_INIT_CLASS(className,classPrintName,parentClass)	      \
    SO__NODE_INIT_CLASS(className,classPrintName,parentClass);                \
    do {          							      \
        parentNodekitCatalogPtr = parentClass::getClassNodekitCatalogPtr();   \
    } while (0)

////////////////////////////////////////////
//
// This calls SO_NODE_INIT_CLASS to initialize the type 
// identifier variables defined in SO_KIT_HEADER.
// It should be called from within initClass(). 
// The parentClass argument should be the class that this 
// subclass is derived from.
// The parentNodekitCatalogPtr is also initialized here.
// 

#define SO_KIT_INIT_CLASS(className,parentClass,parentPrintClass)	      \
    SO_NODE_INIT_CLASS(className,parentClass,parentPrintClass);               \
    do {          							      \
        parentNodekitCatalogPtr = parentClass::getClassNodekitCatalogPtr();   \
    } while (0)

////////////////////////////////////////////
//
// This is an internal macro.
// It initializes the SoNodekitCatalog structure defined in
// SO__KIT_CATALOG_HEADER. This macro is automatically called as 
// part of SO_KIT_CONSTRUCTOR
//
//   This and other macros rely on static member variable "firstInstance"
//   from SoSubNode.h
//

#define SO__KIT_INHERIT_CATALOG(className)	          		      \
    /* get a copy of the catalog from the base class */                       \
    do { 								      \
	if (firstInstance) 						      \
	    if (parentNodekitCatalogPtr == NULL)  /* only true in SoBaseKit */\
		nodekitCatalog = new SoNodekitCatalog;                        \
	    else						      	      \
		nodekitCatalog						      \
	      =(*parentNodekitCatalogPtr)->clone(SoType::fromName(SO__QUOTE(className)));\
    }									      \
    while (0)

////////////////////////////////////////////
//
// These are here to complete the set of subclassing macros for nodekits.
// They are just stubs to call the SO_NODE equivalents, really.

#define SO_KIT_CONSTRUCTOR(className)                                         \
    SO_NODE_CONSTRUCTOR(className);                                           \
    SO__KIT_INHERIT_CATALOG(className)

#define SO_KIT_IS_FIRST_INSTANCE()                                            \
    SO_NODE_IS_FIRST_INSTANCE()

#define SO_KIT_ADD_FIELD(fieldName,defValue)                                  \
    SO_NODE_ADD_FIELD(fieldName,defValue)

#define SO_KIT_DEFINE_ENUM_VALUE(enumType,enumValue)                          \
    SO_NODE_DEFINE_ENUM_VALUE(enumType,enumValue)

#define SO_KIT_SET_MF_ENUM_TYPE(fieldName, enumType)                          \
    SO_NODE_SET_MF_ENUM_TYPE(fieldName, enumType)

#define SO_KIT_SET_SF_ENUM_TYPE(fieldName, enumType)                          \
    SO_NODE_SET_SF_ENUM_TYPE(fieldName, enumType)

////////////////////////////////////////////
//
// This adds the info for a single new part to the SoNodekitCatalog.
// The parameters are as follows:
//
//	partName:     the name used to refer to this part in nodekitCatalog
//	              NOTE: do not make an entry for 'this'.
//                          'this' is implicitly the top of the tree when
//                          building the catalog.
//	partClassName: the class of node to which this part belongs.
//
//	nullByDefault: If TRUE, the part is not created during the constructor.
//                     (which is the usual case.) 
//                     If FALSE, then it will be automatically created by the 
//                     constructor during the call to the 
//                     macro SO_KIT_INIT_INSTANCE()
//
//	parentName:   the partName of the parent of this part within
//                    the nodekitCatalog
//                    NOTE: if this node is to be a direct descendant of 
//                          'this', then parentName should be given as "this"
//
//	rightName:    the partName of the right sibling of this part
//                    within the nodekitCatalog.
//                    NOTE: if this part is to be the rightmost child, then
//                    the rightName should be given as "" (the empty string).
//
//      isPublicPart: can a user receive a pointer to this part through
//                    getPart?  If a part is not a leaf, this property is 
//		      irrelevant (non-leaf parts are always private ). But if 
//                    it is a leaf, the user's access can be stopped through 
//                    this field.
//
// For example,
//
// SO_KIT_ADD_CATALOG_ENTRY(material,SoMaterial,TRUE,this,, TRUE);
//
//       adds to the catalog a part named 'material.'  This part will be an
//       SoMaterial node that is NOT created by default. It will be a direct 
//       child of the nodekit part 'this.' It will be installed as the 
//       rightmost child below 'this'.
//       Since it is public, a user will be able to receive a pointer to this 
//       part by calling getPart().
//
// Another example:
//    if we are making a catalog for a class SoBirdKit, and we have already
//    created the class SoWingKit, then the following macros:
// SO_KIT_ADD_CATALOG_ENTRY(mainSep,SoSeparator,TRUE,this,, FALSE);
// SO_KIT_ADD_CATALOG_ENTRY(rightW,SoWingKit,TRUE,mainSep,, TRUE);
// SO_KIT_ADD_CATALOG_ENTRY(leftW,SoWingKit,TRUE,mainSep,rightWing, TRUE);
//    describe a catalog with this structure:
//
//                  this
//                    |
//                 mainSep
//                    |
//              -------------
//              |           |
//          leftW        rightW 

#define SO_KIT_ADD_CATALOG_ENTRY(partName, partClassName, nullByDefault,      \
		  parentName, rightName, isPublicPart )                       \
   SO_KIT_ADD_FIELD(partName,(NULL));					      \
   if (firstInstance && !nodekitCatalog->addEntry(SO__QUOTE(partName),        \
		    SoType::fromName(SO__QUOTE(partClassName)),               \
		    SoType::fromName(SO__QUOTE(partClassName)), nullByDefault,\
		    SO__QUOTE(parentName),   				      \
	            SO__QUOTE(rightName), FALSE, SoType::badType(),           \
		    SoType::badType(), isPublicPart  ))    		      \
	catalogError()


////////////////////////////////////////////
//
// This adds the info for a new part to the SoNodekitCatalog.
// 'partName' may be of an abstract node type.
//
// The parameters are as follows:
//
//	partName:      same as SO_KIT_ADD_CATALOG_ENTRY
//	partClassName: same as SO_KIT_ADD_CATALOG_ENTRY, except that
//                     abstract node classes are acceptable.
//
//	defaultPartClassName:   If the nodekit is asked to construct this part,
//                          using getPart, then the 'defaultPartClassName' will
//                          specify what type of node to build.
//                          This may NOT be an abstract class.
//                          This MUST be a subclass of 'partClassName'
//
//	nullByDefault: same as SO_KIT_ADD_CATALOG_ENTRY
//	parentName:    same as SO_KIT_ADD_CATALOG_ENTRY
//	rightName:     same as SO_KIT_ADD_CATALOG_ENTRY
//      isPublicPart:  same as SO_KIT_ADD_CATALOG_ENTRY
//
// For example,
//
// SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(light,SoLight, SoDirectionalLight, 
//					TRUE, this,, TRUE);
//
//       makes a part node refered to as "light".
//       When calling setPart, any node that is a subclass of light can be 
//       used (e.g., SoDirectionalLight, SoSpotLight, SoPointLight )
//       However, if the user asks for the node and it has not been created yet,
//          (this happens, for example, in the case where there is currently 
//           no 'light' and the user calls
//           SO_GET_PART( myKit, "light", SoLight ), 
//       then an SoDirectionalLight will be created and returned.
// 

#define SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(partName, partClassName,            \
		  defaultPartClassName, nullByDefault, parentName,            \
		  rightName, isPublicPart )                                   \
   SO_KIT_ADD_FIELD(partName,(NULL));					      \
   if (firstInstance && !nodekitCatalog->addEntry(SO__QUOTE(partName),        \
		    SoType::fromName(SO__QUOTE(partClassName)),               \
		    SoType::fromName(SO__QUOTE(defaultPartClassName)), nullByDefault,\
		    SO__QUOTE(parentName), SO__QUOTE(rightName), FALSE,       \
		    SoType::badType(), SoType::badType(), isPublicPart  ))    \
	catalogError()

////////////////////////////////////////////
//
// This adds the info for a new part to the SoNodekitCatalog.
// 'partName' refers to a part that is a LIST.
//
// Any list in a nodekit will automatically be a node of type SoNodeKitListPart.
// These nodes act like subclasses of group but enforce type checking when 
// you add children to them. 
//
// The parameters you specify will determine 
// [a] what kind of group node holds the children (SoGroup, SoSeparator,
//     SoSwitch, etc).
// [b] what single class of node will be allowable as children in the list.
//
// Subsequent calls to SO_KIT_ADD_LIST_ITEM_TYPE allow you to add other classes
// of nodes that will be allowable children for the list.
//
// The parameters are as follows:
//
//	partName:     Same as in SO_KIT_ADD_CATALOG_ENTRY
//	listContainerClassName:
//                    What class will be used to hold the children in the list?
//                    NOTE: since this is going to have children, it MUST
//                          be a subclass of SoGroup, such as SoSeparator
//                          or SoSwitch.
//	nullByDefault:same as SO_KIT_ADD_CATALOG_ENTRY
//	parentName:   Same as in SO_KIT_ADD_CATALOG_ENTRY
//	rightName:    Same as in SO_KIT_ADD_CATALOG_ENTRY
//
//	listItemClassName: The name of the class of node that may appear in 
//                         the list. Any node class is legal here.
//      isPublicPart: Same as in SO_KIT_ADD_CATALOG_ENTRY
//
// For example,
//
// SO_KIT_ADD_CATALOG_LIST_ENTRY(subCubes,SoSeparator,TRUE,this,,SoCube, TRUE );
//
// makes a list that may contain only SoCubes. An SoSeparator will be used to
// contain this list of cubes.

#define SO_KIT_ADD_CATALOG_LIST_ENTRY(partName, listContainerClassName,       \
      nullByDefault, parentName, rightName, listItemClassName, isPublicPart ) \
    SO_KIT_ADD_FIELD(partName,(NULL));					      \
    if (firstInstance && !nodekitCatalog->addEntry(SO__QUOTE(partName),       \
		  SoNodeKitListPart::getClassTypeId(),                        \
		  SoNodeKitListPart::getClassTypeId(), nullByDefault,         \
		  SO__QUOTE(parentName), SO__QUOTE(rightName), TRUE,          \
		  SoType::fromName(SO__QUOTE(listContainerClassName)),        \
		  SoType::fromName(SO__QUOTE(listItemClassName)), isPublicPart)) \
	catalogError()

////////////////////////////////////////////
//
// Assuming that the part 'partName' was already put in the nodekit catalog,
// using the macro SO_KIT_ADD_CATALOG_LIST_ENTRY(...)
// this macro will add 'newListItemClassName' to its listItemTypes.
//
// This means that nodes of type 'newListItemClassName' will be permitted 
// in the list, as well as any other parts that are already permitted.
// 
// Example:
//    The macro:
//      SO_KIT_ADD_CATALOG_LIST_ENTRY(myList, SoSeparator, TRUE,
//                                         myListParent, , SoCube, TRUE )
//        creates a list called "myList" that accepts cubes.
//        calling:
//           myKit->addChild( "myList", myCube );
//        will work just fine, but:
//           myKit->addChild( "myList", mySphere );
//        will produce an error.
//    If, however, a subsequent call is made to:
//      SO_KIT_ADD_LIST_ITEM_TYPE( myList, SoSphere );
//        then both calls to addChild will be acceptable
//
//  partName:             Name of the part to add the listItemType to
//  newListItemClassName: Name of the class to add to partName's listItemTypes

#define SO_KIT_ADD_LIST_ITEM_TYPE(partName, newListItemClassName ) 	    \
    if (firstInstance) nodekitCatalog->addListItemType(SO__QUOTE(partName), \
	  SoType::fromName(SO__QUOTE(newListItemClassName)) )

////////////////////////////////////////////
//
// This changes the class of a part in the nodekit catalog.
// Both the newPartClassname and the newDefaultPartClassName must be given.
//
// Used when creating catalogs for subclasses of other nodekits.
//
// The parameters are as follows:
//
//	partName:           Same as SO_KIT_ADD_CATALOG_ENTRY
//
//	newPartClassName:   The name of the new node class describing this part.
//
//	newDefaultPartClassName:   
//                          The new defaultPartClassName.  If asked to build a
//                          new node for this part, a node of type 
//                          newDefaultPartClassName will be built.
// For example,
//
// The part "shape" is entered in the nodekit catalog as a part for 
// the SoShapeKit using the following macro call in the constructor:
//
// SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(shape, SoShape, SoCube, FALSE, 
//				     shapeSeparator, , TRUE  );
//
// If we created a subclass of SoShape, SoVertexShapeKit, 
// then the following command might be used in the SoVertexShapeKit constructor:
//
// SO_KIT_CHANGE_ENTRY_TYPE( shape, SoVertexShape, SoFaceSet );
//
// This would allow any subclass of SoVertexShape to be inserted as the part
// "shape."   If the SoVertexShapeKit were asked to build the part "shape", it
// would create it as an SoFaceSet, the default (and non-abstract) type.
//
// Continuing further, the class SoFaceSetKit, subclassed from SoVertexShapeKit,
// might contain this command in its constructor:
//
// SO_KIT_CHANGE_ENTRY_TYPE(shape,SoFaceSet, SoFaceSet );
//
// This class would allow one and only one type of node for "shape", 
// and that is the type SoFaceSet.

#define SO_KIT_CHANGE_ENTRY_TYPE(partName, newPartClassName,                \
			         newDefaultPartClassName)                   \
   if (firstInstance) nodekitCatalog->narrowTypes(SO__QUOTE(partName),      \
	   SoType::fromName(SO__QUOTE(newPartClassName)),                     \
	   SoType::fromName(SO__QUOTE(newDefaultPartClassName)))

////////////////////////////////////////////
//
// This changes whether or not a part is NULL by default.
// If TRUE, the part is NULL by default.
// If FALSE, the part is always created during the constructor for the
//     nodekit that uses this catalog. In this case, construction occurs 
//     during the call to SO_KIT_INIT_INSTANCE().
//
#define SO_KIT_CHANGE_NULL_BY_DEFAULT(partName, newNullByDefault)             \
   if (firstInstance) nodekitCatalog->setNullByDefault(SO__QUOTE(partName),   \
        newNullByDefault)

////////////////////////////////////////////
//
// This must be called within the constructor of every class of nodekit.
// It should be called immediately after the catalog is defined.
// 
// It does 2 things:
// [1] created the nodekitPartsList, which keeps track of which parts
//     have been created.
// [2] creates all parts that must be created by default (such as the
//	cube in SoCubeKit) as specified by the nullByDefault parameter
//  	in the nodekit catalog
//
#define SO_KIT_INIT_INSTANCE()   \
   createNodekitPartsList(); \
   createDefaultParts()

// C-api: begin

#endif /* _SO_SUB_KIT_ */
