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
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	many subclasses of SoV1BaseKit. They may be used to make SoV1BaseKit
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the baseKit subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |		SO_V1_SUBKIT_CATALOG_HEADER
 |
 |	Within class source:
 |		SO_V1_SUBKIT_CATALOG_VARS
 |		SO_V1_SUBKIT_CATALOG_METHODS
 |		SO_V1_SUBKIT_INHERIT_CATALOG
 |		SO_V1_SUBKIT_ADD_CATALOG_ENTRY
 |		SO_V1_SUBKIT_ADD_CATALOG_ABSTRACT_ENTRY
 |              SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY
 |              SO_V1_SUBKIT_ADD_LIST_ITEM_TYPE
 |              SO_V1_SUBKIT_CHANGE_ENTRY_TYPE
 |
 |   Author(s)		: Paul Isaacs, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_V1_SUB_KIT_
#define  _SO_V1_SUB_KIT_

#include <Inventor/nodes/SoNode.h>
#include <Inventor/misc/upgraders/SoV1NodekitCatalog.h>


/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for a SoV1BaseKit
// subclass:
//

////////////////////////////////////////////
//
// This defines a catalog describing the arrangement 
// of the subgraph for instances of this class
//

#define SO_V1_SUBKIT_CATALOG_HEADER(className)				    \
  private:								    \
    static SoV1NodekitCatalog *nodekitCatalog; /* design of this class */   \
  protected:								    \
    static SoV1NodekitCatalog *getClassNodekitCatalog();		    \
    /* Returns an SoV1NodekitCatalog for the node */			    \
    virtual const SoV1NodekitCatalog *getNodekitCatalog() const


/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a node subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_V1_SUBKIT_CATALOG_HEADER.
//

#define SO_V1_SUBKIT_CATALOG_VARS(className)				      \
SoV1NodekitCatalog   *className::nodekitCatalog = NULL

////////////////////////////////////////////
//
// This implements the methods defined in SO_V1_SUBKIT_CATALOG_HEADER.
//

#define SO_V1_SUBKIT_CATALOG_METHODS(className)				    \
const SoV1NodekitCatalog *						    \
className::getNodekitCatalog() const					    \
{									    \
    return nodekitCatalog;						    \
}									    \
SoV1NodekitCatalog *							    \
className::getClassNodekitCatalog()					    \
{									    \
    return nodekitCatalog;						    \
}

////////////////////////////////////////////
//
// This initializes the SoV1NodekitCatalog structure defined in
// SO_V1_SUBKIT_CATALOG_HEADER. This macro should be called from within
// initClass().
//

#define SO_V1_SUBKIT_INHERIT_CATALOG(className, parentClassName )	      \
    /* get a copy of the catalog from the base class */                     \
    if ( !parentClassName::getClassNodekitCatalog() )                       \
	parentClassName::initClass();                                       \
    nodekitCatalog                                                           \
    = (parentClassName::getClassNodekitCatalog())->clone(                   \
					       className::getClassTypeId() )

////////////////////////////////////////////
//
// This adds the info for a node to the SoV1NodekitCatalog for a single node. 
// The parameters are as follows:
//
//	name:	      the name used to refer to this node in nodekitCatalog
//	              NOTE: do not make an entry for 'this'.
//                          'this' is implicitly the top of the tree when
//                          building the catalog.
//	className:    the class of node to which this node belongs.
//
//	parentName:   the reference name of the parent of this node within
//                    the nodekitCatalog
//                    NOTE: if this node is to be a direct descendant of 
//                          'this', then parentName should be given as "this"
//	rightName:    the reference name of the right sibling of this node
//                    within the nodekitCatalog.
//                    NOTE: if this node is to be the rightmost child, then
//                    the rightName should be given as "" (the empty string).
//      isPublicPart: can a user receive a pointer to this part through
//                    getPart?  If a part is not a leaf, this property is 
//		      irrelevant (other parts are always private ). But if it 
//                    is a leaf, the user's access can be stopped through 
//                    this field.
//
// For example,
//
// SO_V1_SUBKIT_ADD_CATALOG_ENTRY("material",SoMaterial,"this","", TRUE);
//       makes a material node refered to as "material" as a direct child of
//       the nodekit node 'this.' It will be installed as the rightmost child.
//       A user will be able to receive a pointer to this part.
//
// Another example:
//    if we are making a catalog for a class SoBirdKit, and we have already
//    created the class SoWingKit, then the following macros:
// SO_V1_SUBKIT_ADD_CATALOG_ENTRY("mainSep",SoSeparator,"this","", FALSE);
// SO_V1_SUBKIT_ADD_CATALOG_ENTRY("rightW",SoWingKit,"mainSep","", TRUE);
// SO_V1_SUBKIT_ADD_CATALOG_ENTRY("leftW",SoWingKit,"mainSep","rightWing", TRUE);
//    describe a catalog with this structure:
//
//                  this
//                    |
//              -------------
//              |           |
//          leftW        rightW 

#define SO_V1_SUBKIT_ADD_CATALOG_ENTRY(name, className, parentName,          \
	          rightName, isPublicPart )                               \
   nodekitCatalog->addEntry(name, className::getClassTypeId(),            \
		    className::getClassTypeId(), parentName,              \
		  rightName, FALSE, SoType::badType(), isPublicPart  )

////////////////////////////////////////////
//
// This adds the info for a node of abstract type to the SoV1NodekitCatalog 
// for a single node. 
//
// The parameters are as follows:
//
//	name:	      same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//	className:    same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY, except that
//                    abstract node classes are acceptable.
//
//	defaultClassName:   If the nodekit is asked to construct this part,
//                          using getPart, then the 'defaultClassName' will
//                          specify what type of node to build.
//                          This may NOT be an abstract class.
//                          This MUST be a subclass of 'className'
//
//	parentName:   same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//	rightName:    same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//      isPublicPart: same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//
// For example,
//
// SO_V1_SUBKIT_ADD_CATALOG_ABSTRACT_ENTRY("light",SoLight, SoDirectionalLight, 
//					"this","", TRUE);
//
//       makes a part node refered to as "light".
//       When calling setPart, any node that is a subclass of light can be 
//       used (e.g., SoDirectionalLight, SoSpotLight, SoPointLight )
//       However, if the user asks for the node (this happens, for example, 
//       if the case where there is currently no 'light' and the user calls
//       GETPART( myKit, "light", SoLight ), then a Directional light
//       will be created and returned.
// 

#define SO_V1_SUBKIT_ADD_CATALOG_ABSTRACT_ENTRY(name, className, defaultClassName,\
		  parentName,  rightName, isPublicPart )                       \
   nodekitCatalog->addEntry(name, className::getClassTypeId(),                 \
		    defaultClassName::getClassTypeId(), parentName,            \
		  rightName, FALSE, SoType::badType(), isPublicPart  )

////////////////////////////////////////////
//
// This adds the info for a node list to the SoV1NodekitCatalog 
// A node list is a group node that is forced (by the nodekit) to contain 
// children all of the same type.
// The parameters are as follows:
//
//	name:	      Same as in SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//	className:    Same as in SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//                    EXCEPT!! 
//                    NOTE: since this is going to have children, it must
//                          be an SoGroup or SoSeparator.
//	parentName:   Same as in SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//
//	rightName:    Same as in SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//	listItemClassName:    The name of the type of node to appear in the
//                            list. Anything is legal here.
//      isPublicPart: can a user perform operations on the children of this
//                    list? 
//
// For example,
//
// SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("subCubes",SoSeparator,"this","",
//					SoCube, TRUE );
//
// makes a Separator node that will later be a list of SoCubes.

#define SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY(name, className, parentName,     \
	          rightName, listItemClassName, isPublicPart )            \
    nodekitCatalog->addEntry(name, className::getClassTypeId(),           \
		  className::getClassTypeId(), parentName,                \
		  rightName, TRUE, listItemClassName::getClassTypeId(),   \
		  isPublicPart )

////////////////////////////////////////////
//
// Assuming that the part 'name' already exists, this macro
// will add 'newClass' to its listItemTypes.
//
// This means that (assuming 'name' is entered into the catalog as a 
// list), nodes of type 'newClass' will be permitted in the list.
// 
// Example:
//    The macro:
//      SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("myList", SoSeparator, 
//                                         "myListParent", "", SoCube, TRUE )
//        creates a list called "myList" that accepts cubes.
//        calling:
//           myKit->addChild( "myList", myCube );
//        will work just fine, but:
//           myKit->addChild( "myList", mySphere );
//        will produce an error.
//    If, however, a subsequent call is made to:
//      SO_V1_SUBKIT_ADD_LIST_ITEM_TYPE( "myList", SoSphere );
//        then both calls to addChild will be acceptable
//
//	name:	      Name of the part to add the listItemType to
//	newClass:     Name of the class to add to 'names' listItemTypes

#define SO_V1_SUBKIT_ADD_LIST_ITEM_TYPE(name, newClass ) 		    \
    nodekitCatalog->addListItemType(name, newClass::getClassTypeId() )

////////////////////////////////////////////
//
// This changes the node-type of a part.
// Both the type and the default type must be given.
//
// Used when creating catalogs for subclasses of other nodekits.
//
// The parameters are as follows:
//
//	name:	      same as SO_V1_SUBKIT_ADD_CATALOG_ENTRY
//	newClassName: the new className describing this part.
//
//	newDefaultClassName:   The new defaultClassName.  If asked to build a
//                          new node for this part, a node of type 
//                          newDefaultClassName will be built.
// For example,
//
// If the part "shape" was originally created as part of the SoV1ShapeKit,
// using the following command (within initClass()):
//
// SO_V1_SUBKIT_ADD_CATALOG_ABSTRACT_ENTRY("shape",SoShape, SoSphere, 
//					"this","", TRUE);
//
// and we are creating the subclass SoV1VertexShapeKit,
// then the following command will be used (within initClass()):
//
// SO_V1_SUBKIT_CHANGE_ENTRY_TYPE("shape",SoVertexShape, SoFaceSet );
//
// This means that in the SoV1VertexShapeKit class, any node used for "shape" 
// must be derived from SoVertexShape. 
// If the SoVertexShape is asked to build the part "shape", it will create
// an SoFaceSet node, by default.
// 
// 
// Continuing further, the class SoV1FaceSetKit, subclassed from SoV1VertexShapeKit,
// will contain this command (within initClass()):
//
// SO_V1_SUBKIT_CHANGE_ENTRY_TYPE("shape",SoFaceSet, SoFaceSet );
//
// This class is a leaf class, and only one type of node is acceptable as 
// "shape", and that is the type SoFaceSet.

#define SO_V1_SUBKIT_CHANGE_ENTRY_TYPE(name, newClassName, newDefaultClassName)  \
   nodekitCatalog->narrowTypes(name, newClassName::getClassTypeId(),          \
		                     newDefaultClassName::getClassTypeId())


#endif /* _SO_V1_SUB_KIT_ */
