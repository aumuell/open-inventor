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
 |      Defines the SoNodekitParts class. An instance of this class is
 |      created for each instance of an Nodekit.
 |      Its function is to describe which 'parts' the nodekit has created
 |      from its class' SoNodekitCatalog.
 |      An SoNodekitParts contains a pointer to an SoNodekitCatalog and 
 |      a correlated list of nodes.
 |
 |      If a new part is asked for, this class uses the catalog as a 
 |      guide to create the part.
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_NODEKIT_PARTS
#define  _SO_NODEKIT_PARTS

#include <Inventor/nodekits/SoNodekitCatalog.h>

class SoNodeKitListPart;
class SoBaseKit;
class SoNode;
class SoFullPath;
class SoSFNode;

////////////////////////////////////////////////////////////////////
//    Class: SoNodekitParts
////////////////////////////////////////////////////////////////////
SoEXTENDER class SoNodekitParts {

  SoEXTENDER public:
    // Instances of SoNodekitParts are created only by subclasses of SoBaseKit.

    // Constructor -- requires a catalog to build
    SoNodekitParts( SoBaseKit *rootOfKit );

    // Destructor
    ~SoNodekitParts();

    friend class SoBaseKit;

  private:
    const SoNodekitCatalog *catalog;   // the catalog to use in building
					  // parts
    int			    numEntries;   // must be made equal to numEntries
					  // in the catalog
    SoBaseKit               *rootPointer; // Pointer to the part 'this'
    SoSFNode                **fieldList;  // each 'part' is actually just
					  // a node, stored in a field

    SbBool makePart( const int partNum );
    SbBool replacePart( const int partNum, SoNode *newPartNode );
    void   unregisterDescendants( const int partNum );

    SoNode *getPartFromThisCatalog( const int partNum, SbBool makeIfNeeded,
				      SbBool leafCheck, SbBool publicCheck, 
				      SbBool &existedBefore );
    SoFullPath *createPathToPartFromThisCatalog( const int partNum, 
		    SbBool makeIfNeeded, SbBool leafCheck, 
		    SbBool publicCheck,  SbBool &existedBefore );
    SbBool setPartFromThisCatalog( const int partNum, 
				     SoNode *newPartNode,
				     SbBool anyPart );

    SoNode *getSingleNamePart( const SbName &nameOfPart, SbBool makeIfNeeded,
				SbBool leafCheck, SbBool publicCheck, 
				SbBool &existedBefore );
    SoFullPath *getSingleNamePathToPart( const SbName &nameOfPart, 
				 SbBool makeIfNeeded, SbBool leafCheck, 
				 SbBool publicCheck, SbBool &existedBefore );
    SbBool setSingleNamePart( const SbName &nameOfPart, SoNode *newPartNode,
				SbBool anyPart );

    // returns the part requested. If 'leafCheck' then it will only return 
    // leaves. If 'publicCheck', then it will only return public parts.
    // Otherwise it will also return interior parts.
    // Interior parts are ordinarily not obtainable through public methods.
    SoNode *getAnyPart( const SbName &nameOfPart, SbBool makeIfNeeded, 
			SbBool leafCheck, SbBool publicCheck );
    SoNode *getAnyPart( const SbName &nameOfPart, SbBool makeIfNeeded, 
			SbBool leafCheck, SbBool publicCheck, 
			SbBool &existedBefore);
    SoFullPath *createPathToAnyPart( const SbName &nameOfPart, 
		    SbBool makeIfNeeded, SbBool leafCheck, SbBool publicCheck );
    SoFullPath *createPathToAnyPart( const SbName &nameOfPart, 
		    SbBool makeIfNeeded, SbBool leafCheck, 
		    SbBool publicCheck,  SbBool &existedBefore );
    SbBool setAnyPart(  const SbName &nameOfPart, SoNode *newPartNode, 
			SbBool anyPart);

    // parse a string like 'nameOfList[elementNum]' into two parts...
    // Destructive to parseString, so give it a copy if you care.
    // Replaces the '[' with an end-of-string and reads elementNum 
    // into the integer given
    SbBool parseListItem( char *parseString, int &index );

    // help to delete unneeded parts...
    SbBool partIsNonLeafAndMayBeDeleted( int partNum );

    // error check functions...
    SbBool partFoundCheck( int partNum );
    SbBool partIsLeafCheck( int partNum );
    SbBool partIsPublicCheck( int partNum );
    SbBool verifyPartExistence( int partNum );

    // builds a path from the entry 'this' down to the entry given
    // by 'nameOfPart'. Assumes that 'theNode' matches 'nameOfPart'
    SoFullPath *createPathDownTo( const SbName &nameOfPart, SoNode *theNode );

    // appends pathB after pathA and returns result.
    // result is NULL if either path is NULL or if pathA->tail != pathB->head
    SoFullPath *addPaths( const SoFullPath *pathA, 
			    const SoFullPath *pathB ) const;
};

#endif  /* _SO_NODEKIT_PARTS */
