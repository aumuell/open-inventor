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
 |      Defines the SoBaseKit class. A base class for all
 |      of the SoNodeKit classes
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_BASE_KIT_
#define  _SO_BASE_KIT_

#include <Inventor/misc/SoChildList.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodekits/SoNodekitParts.h>
#include <Inventor/nodekits/SoSubKit.h>
#include <Inventor/SoNodeKitPath.h>

class SoSeparator;
class SbBox3f;
class SbDict;
class SoGroup;

////////////////////////////////////////////////////////////////////
//    Class: SoBaseKit -- 
//
//  This is the base class for all node kits.
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoBaseKit:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoBaseKit.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//  It contains only 'this' and a callbackList beneath it.
//  It's catalog is made to describe the following structure:
//
//                            this
//                              |
//                            "callbackList"
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoKit
class SoBaseKit : public SoNode {

    // Instead of calling SO_KIT_HEADER(className) here,
    // which is the norm for all nodekits, explicitly write the contents
    // of that macro, so as to expose the base class virtual function
    // getNodekitCatalog for the C-api...
    //
    SO_NODE_HEADER(SoBaseKit);

  private:
    static SoNodekitCatalog *nodekitCatalog; /* design of this class */
    static const SoNodekitCatalog **parentNodekitCatalogPtr; /* parent design */
  public:
    // C-api: name=getClassNkitCat
    static const SoNodekitCatalog *getClassNodekitCatalog()
	{ return nodekitCatalog; }

    /* Returns an SoNodekitCatalog for the node */
    // C-api: expose
    // C-api: name=getCat
    virtual const SoNodekitCatalog *getNodekitCatalog() const;
    // ...end of contents of SO_KIT_HEADER

  protected:
    static const SoNodekitCatalog **getClassNodekitCatalogPtr()
	{ return (const SoNodekitCatalog **)&nodekitCatalog; }

    // Define fields for the new parts of the kit...
    SO_KIT_CATALOG_ENTRY_HEADER(callbackList);

  public:

    //
    // CONSTRUCTOR
    //
    SoBaseKit();

    //
    // GETTING PARTS AND PATHS
    //
    // If no node is present, NULL will be returned, unless makeIfNeeded = TRUE
    // If makeIfNeeded = TRUE,
    // -- creates the node 
    // -- looks in the catalog to see if any extra nodes are needed to
    //    connect the node up the nodekit to 'this'
    //
    // This method is virtual so subclasses may do extra things
    // when certain parts are requested.
    //
    // Prints an error if the environment variable IV_DEBUG_KIT_PARTS is set 
    // and 'partName' can't be found in catalog of this or any derived part.
    // C-api: expose
    virtual SoNode *getPart( const SbName &partName, SbBool makeIfNeeded );

    // Given a node or a path to a node, sees if it's a part in the 
    // nodekit, a part in a nested nodekit, or an element of a list part.
    // If so, returns a string containing the name. 
    // If not, returns the empty string.
    SbString getPartString( const SoBase *part );
		     
		     
    // createPathToPart returns a path that begins with 'this', and ends at 
    // the part named.  
    // This method is virtual so subclasses may do extra things
    // when certain parts are requested.
    //
    // Prints an error if the environment variable IV_DEBUG_KIT_PARTS is set 
    // and 'partName' can't be found in catalog of this or any derived part.
    // C-api: expose
    virtual SoNodeKitPath *createPathToPart( const SbName &partName, 
		    SbBool makeIfNeeded, const SoPath *pathToExtend = NULL );
		   
		   
    // 
    // SETTING PARTS
    //
    // 'set' method inserts the given node (not a copy!) as the new part
    //       specified by partName
    //
    // Adds any extra nodes needed to fit the part into the nodekit's structure
    //
    // --an argument of NULL means to remove the specified node.
    //
    // This is virtual so subclasses may add functionality,
    // such as setting a field if the partName is a particular part.
    //
    // Prints an error if the environment variable IV_DEBUG_KIT_PARTS is set 
    // and 'partName' can't be found in catalog of this or any derived part.
    // C-api: expose
    virtual SbBool setPart( const SbName &partName, SoNode *from );

    //
    // SETTING FIELDS OF PARTS
    //
    // set routine returns FALSE if it cannot find the parameter to set
    SbBool set(char *nameValuePairListString);
    // C-api: name=set2
    SbBool set(char *partNameString, char *parameterString);

    // DO NODEKITS TRAVERSE THEIR CHILDREN DURING A SEARCH ACTION?
    // By default, they do NOT.
    static SbBool isSearchingChildren() { return searchingChildren; }
    static void   setSearchingChildren( SbBool newVal );

  SoEXTENDER public:
    virtual void doAction( SoAction *action );
    // These functions implement all actions for nodekits.
    virtual void callback( SoCallbackAction *action );
    virtual void GLRender( SoGLRenderAction *action );
    virtual void getBoundingBox( SoGetBoundingBoxAction *action );
    virtual void getMatrix(SoGetMatrixAction *action );
    virtual void handleEvent( SoHandleEventAction *action );
    virtual void rayPick( SoRayPickAction *action );
    virtual void search( SoSearchAction *action );
    virtual void write( SoWriteAction *action );

  SoINTERNAL public:
    static void initClass();

    // Returns pointer to children
    virtual SoChildList *getChildren() const;

    static SoNode *typeCheck( const SbName &partName, const SoType &partType, 
			      SoNode *node );

    void printDiagram();
    void printSubDiagram( const SbName &rootName, int level );
    void printTable();

    // Overrides the default method to use countMyFields() instead of 
    // the regular SoFieldData writing mechanism.
    virtual void	addWriteReference(SoOutput *out,
					  SbBool isFromField = FALSE);

    friend class SoNodekitCatalogEntry;
    friend class SoNodekitParts;
    friend class SoV1BaseKit;

    // This returns TRUE if the nodekit intends to write out.
    //   [a] call shouldWrite(). If TRUE, trivial return.
    //   [b] If the kit thinks it shouldn't write, it first does a recursive
    //       call to its children.  If any children must write, then so must
    //       the kit.
    //   [c] If kit has changed its mind because of [b], then add a writeRef.
    //
    //   [d] If kit should not write, it will delete the fieldDataForWriting, 
    //       since there will no writing pass applied to take care of this.
    SbBool forceChildDrivenWriteRefs( SoOutput *out );

  protected:

    SoChildList *children;

    // Redefines this to add this node and all part nodes to the dictionary
    virtual SoNode *	addToCopyDict() const;

    // Copies the contents of the given nodekit into this instance
    virtual void	copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

    // Returns the containerNode within the SoNodeKitListPart given 
    // by listName.
    SoGroup *getContainerNode( const SbName &listName,
			       SbBool makeIfNeeded = TRUE );

    // like their public versions, but are allowed access to non-leaf
    // and private parts.
    // These are virtual so subclasses may do extra things
    // when certain parts are requested.
    virtual SoNode *getAnyPart( const SbName &partName, SbBool makeIfNeeded, 
			SbBool leafCheck = FALSE, SbBool publicCheck = FALSE );
    virtual SoNodeKitPath *createPathToAnyPart(const SbName &partName, 
		    SbBool makeIfNeeded, 
		    SbBool leafCheck = FALSE, SbBool publicCheck = FALSE,
		   const SoPath *pathToExtend = NULL );
    virtual SbBool setAnyPart( const SbName &partName, SoNode *from, 
		       SbBool anyPart = TRUE );

    // parts list.
    SoNodekitParts	*nodekitPartsList;

    // called during construction to create parts list
    void createNodekitPartsList();
    // called during construction to create any parts that are created by
    // default (such as the cube in the SoCubeKit)
    virtual void createDefaultParts();

    // Return the node's partsList
    const SoNodekitParts *getNodekitPartsList() const 
			    { return nodekitPartsList; };

    // Prints an Error when there's trouble building a catalog.
    void catalogError();

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copyContents.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor to add their own connections to the ones already
    // connected by the base classes.
    // The doItAlways flag can force the method to do the work.
    // But if (doItAlways == FALSE && onOff == connectionsSetUp), then
    // the method will return immediately without doing anything.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );
    SbBool connectionsSetUp;

    // Reads in from file. Takes care of setting parts and stuff.
    virtual SbBool readInstance(SoInput *in, unsigned short flags);
    
    // This is called during countMyFields() method. It calls setDefault()
    // on those fields we do not wish to write out. setDefault does not change
    // the field value, but it sets a flag indicating that the field should
    // not get written by this node.
    // Fields that we do not write are [a] parts that are NULL and are defined
    // in the catalog to be NULL by default. [b] non-leaf parts for which
    // isNodeFieldValuesImportant() is FALSE.
    // [c] leaf parts that are empty groups, empty separators, 
    // [d] leaf parts that are empty lists (but they must have group or 
    //     separator container nodes.
    virtual void setDefaultOnNonWritingFields();

    // This is called during addWriteReference() to write the fields.
    // It sometimes treats fields for parts of the nodekit in a special way.
    // Under normal circumstances, they write like other fields.
    // The special case is when a part-field has a non-NULL value, but has 
    // also been set to default, indicating that we would rather not write it.
    // If the part is a regular node, we give it a field-connection style
    // write ref.  So it only writes if some other instance of the node forces
    // a write.
    // If the part is a nodekit, then we additionally write ref the fields of 
    // the nodekit, using this same method.
    // Later, in the WRITE stage, the kit-part will be written if at least
    // one of its fields or ancestor parts has shouldWrite() ==TRUE.
    // This way, if a nodekit part or any of its ancestors has non-NULL fields,
    // it will later be forced to write, even though it has been set to default.
    // Example: 
    //   Parent draggers attempt not to write out child draggers.
    //   But the parentDragger must at least traverse the childDragger to see 
    //   if any of the part geometry has been changed from its default. Such 
    //   changes must be written to file.
    void countMyFields(SoOutput *out);

    virtual ~SoBaseKit();

  private:
    void skipWhiteSpace(char *&string);

    // Returns FALSE if every field has either [a] hasDefaultValue() == TRUE or
    // [b] isDefault() == TRUE
    SbBool isNodeFieldValuesImportant( SoNode *n );

    // This creates a copy of fieldData with a few changes that make
    // it write out 'prettier'
    void createFieldDataForWriting();
    // Used to store field data during writing. Need this because a temporary
    // version of fieldData is made that differs from the real thing.
    SoFieldData *fieldDataForWriting;

    // Called by the readInstance routine...
    SbBool readMyFields(SoInput *in, SoFieldData *&unknownFieldData );

    // Called by write() after the (virtual) setDefaultOnNonWritingNodes() 
    // method.  This method looks at the part fields which have isDefault()
    // set to TRUE.    This method will setDefault(FALSE) on any part fields
    // that MUST write.   
    //     This happens when the part-field is for a part whose parent 
    //     is going to write out anyway. Therefore, it will appear in file as 
    //     a node within this kit, so we better write out the part field to 
    //     explain where the node belongs in the kit.
    void undoSetDefaultOnFieldsThatMustWrite();

    // Only nodekits themselves can access the private children.
    int  getNumChildren() const { return (children->getLength()); }
    void removeChild( int index );
    void removeChild( SoNode *child  ) { removeChild(findChild(child)); }
    void addChild( SoNode *child );
    int  findChild( const SoNode *child ) const;
    void insertChild( SoNode *child, int newChildIndex );
    SoNode *getChild( int index) const { return (*children)[index]; }
    void replaceChild( int index, SoNode *newChild);
    void replaceChild( SoNode *oldChild, SoNode *newChild)
	{ replaceChild(findChild(oldChild),newChild); }

    static SbBool searchingChildren;
};

// Macro for getting at parts without doing type casting yourself:
// for example, the code:
//  xf = (SoTransform *) myKit->getPart("transform",
//					SoTransform::getClassTypeId(), TRUE);
// becomes:
//  xf = SO_GET_PART( myKit, "transform", SoTransform );
//      
// The macro SO_CHECK_PART will not build the part if it is not already in the
// kit, since it sends 'FALSE' as the 'makeIfNeeded' argument.
//
// CAUTION:  These macros will not work in the C-api, and nothing will be
//           gained from making them.  So, don't make them.


// C-api: end
#ifdef DEBUG
#define SO_GET_PART( kitContainingPart, partName, partClassName )              \
        ((partClassName *) SoBaseKit::typeCheck( partName,                     \
			       partClassName::getClassTypeId(),               \
			       kitContainingPart->getPart( partName, TRUE )))

#define SO_CHECK_PART( kitContainingPart, partName, partClassName )            \
        ((partClassName *) SoBaseKit::typeCheck( partName,                    \
			       partClassName::getClassTypeId(),               \
			       kitContainingPart->getPart( partName, FALSE )))

#define SO_GET_ANY_PART( kitContainingPart, partName, partClassName )         \
        ((partClassName *) SoBaseKit::typeCheck( partName,                    \
		partClassName::getClassTypeId(),                              \
		kitContainingPart->getAnyPart( partName, TRUE, FALSE, FALSE )))

#define SO_CHECK_ANY_PART( kitContainingPart, partName, partClassName )       \
        ((partClassName *) SoBaseKit::typeCheck( partName,                    \
		partClassName::getClassTypeId(),                              \
		kitContainingPart->getAnyPart( partName, FALSE, FALSE, FALSE )))
#else

#define SO_GET_PART( kitContainingPart, partName, partClassName )             \
        ((partClassName *) kitContainingPart->getPart( partName, TRUE ))
#define SO_CHECK_PART( kitContainingPart, partName, partClassName )           \
        ((partClassName *) kitContainingPart->getPart( partName, FALSE ))
#define SO_GET_ANY_PART( kitContainingPart, partName, partClassName )        \
        ((partClassName *) kitContainingPart->getAnyPart( partName, TRUE,    \
							  FALSE, FALSE ))
#define SO_CHECK_ANY_PART( kitContainingPart, partName, partClassName )      \
        ((partClassName *) kitContainingPart->getAnyPart( partName, FALSE,   \
							  FALSE, FALSE ))
#endif
// C-api: begin

#endif  /* _SO_BASE_KIT_ */
