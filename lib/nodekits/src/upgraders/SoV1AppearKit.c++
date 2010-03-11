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
 |      SoV1AppearanceKit
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1AppearanceKit.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/errors/SoDebugError.h>


// Define the required type id and name inquiry methods
SO_NODE_SOURCE(SoV1AppearanceKit);

// Define the required catalog inquiry methods
SO_V1_SUBKIT_CATALOG_VARS(SoV1AppearanceKit);
SO_V1_SUBKIT_CATALOG_METHODS(SoV1AppearanceKit);

SoNodeList *SoV1AppearanceKit::kitsWithUnusedTextureXfs = NULL;
SoNodeList *SoV1AppearanceKit::unusedTextureXfs = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1AppearanceKit::SoV1AppearanceKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1AppearanceKit);

    if(SO_NODE_IS_FIRST_INSTANCE() ) {
    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use ...INHERIT_CATALOG once, and put it first.
    // Then, use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.
    SO_V1_SUBKIT_INHERIT_CATALOG(SoV1AppearanceKit, SoV1BaseKit );
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("lightModel",  SoLightModel, "this", "",TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("environment", SoEnvironment,"this", "",TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("drawStyle",   SoDrawStyle,  "this", "",TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("material",    SoMaterial,   "this", "",TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("complexity",  SoComplexity, "this", "",TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("texture2List",  SoGroup,  
				      "this", "", SoTexture2, TRUE );
    // NOTE: This differs from the AppearanceKit in V1.0 and V2.0
    //       This is because a texture node can get converted into
    //       a group containing both a texture and texture2Transform by
    //       the SoV1Texture2 upon readin. We don't want to throw it away.
    //       Instead, we hold it as a group and extract the texture during
    //       SoV1AppearanceKit::dealWithUpgradedPart().
    SO_V1_SUBKIT_ADD_LIST_ITEM_TYPE("texture2List",  SoGroup );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("font",        SoFont,     "this", "",TRUE );

    }

    // Create the static variables:
    if ( kitsWithUnusedTextureXfs == NULL)
	 kitsWithUnusedTextureXfs = new SoNodeList;
    if ( unusedTextureXfs == NULL )
	 unusedTextureXfs = new SoNodeList;
    
    

    createNodekitPartsList();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoV1AppearanceKit::~SoV1AppearanceKit()
//
////////////////////////////////////////////////////////////////////////
{
}

SoNode *
SoV1AppearanceKit::createNewNode()
{
    return new SoAppearanceKit;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    A virtual function that allows classes to treat parts that fail the
//    tests of 'tryToSetPartInNewNode'
//
// If tryToSetPartInNewNode fails, then this routine is called.
// It will fail if the part read from file was called "texture2List"
// This part has been changed to a single noded part, "texture2"
// We will use just the first child of the list and set it as the "texture2"
//
// Use: public
//
SbBool 
SoV1AppearanceKit::dealWithUpgradedPart( SoBaseKit *newNode, SoNode *newPart, 
				    const SbName &newPartName )
//
////////////////////////////////////////////////////////////////////////
{
    // First, try to let base class handle it...
    if ( SoV1BaseKit::dealWithUpgradedPart( newNode, newPart, newPartName ) )
	return TRUE;

    // If the part name is "texture2List", get the first element
    // and set it in the new kit as "texture2"
    if ( newPartName == "texture2List" ) {
	if ( !newPart->isOfType( SoGroup::getClassTypeId() ))
	    return FALSE;

	SoGroup *listGrp = (SoGroup *) newPart;
	if (listGrp->getNumChildren() > 0 ) {
	    SoNode *myNode = listGrp->getChild(0);
	    SoTexture2 *myTexture2 = NULL;
	    if ( !myNode->isOfType( SoTexture2::getClassTypeId() )) {
		// The texture2 upgrader returns a group with an SoTexture2
		// as a child in some cases.  If so, replace it with just
		// the texture and use that texture,
		// otherwise just return.
		if ( myNode->isOfType( SoGroup::getClassTypeId() ) ) {
		    SoGroup *tex2UpgdGrp = (SoGroup *) myNode;
		    tex2UpgdGrp->ref();
		    for (int i = 0; i < tex2UpgdGrp->getNumChildren(); i++ ) {
			SoNode *theKid = tex2UpgdGrp->getChild(i);
			if (theKid->isOfType(SoTexture2::getClassTypeId())) {
			    
			    // Use the texture child.
			    myTexture2 = (SoTexture2 *) theKid;

			    // Replace the texture child for its parent group:
			    listGrp->replaceChild( tex2UpgdGrp, myTexture2 );

	    SoDebugError::postWarning("SoV1AppearanceKit::dealWithUpgradedPart",
		"the input file contained a texture2 with transform fields in it. Since the new SoTexture2 does not contain these fields they may be lost");

			}
			if (theKid->isOfType(SoTexture2Transform::getClassTypeId())) {
			    // Save the values of newNode and theKid 
			    // in case a parent
			    // nodekit wants to get at it later.
			    // This is necessary because AppearanceKit is not
			    // capable of storing the info for texture transform
    			    kitsWithUnusedTextureXfs->append(newNode);
    			    unusedTextureXfs->append(theKid);
			}
		    }
		    tex2UpgdGrp->unref();
		}
		if (myTexture2 == NULL)
		    return FALSE;
	    }
	    else {
	        myTexture2 = (SoTexture2 *) myNode;
	    }
	    SoDebugError::postWarning("SoV1AppearanceKit::dealWithUpgradedPart",
		"the input file contained a texture2List part. The new kits have only a texture2 part. Setting texture2 to be the first child in texture2List");
	    newNode->setPart("texture2", myTexture2 );
	}
	return TRUE;
    }

    return FALSE;   // don't know how to do anything yet...
}

////////////////////////////////////////////////////////////////////////
//
// Use: static, INTERNAL public
//
SoNodeList * 
SoV1AppearanceKit::getKitsWithUnusedTextureXfs()
//
////////////////////////////////////////////////////////////////////////
{
    return kitsWithUnusedTextureXfs;
}

////////////////////////////////////////////////////////////////////////
//
// Use: static, INTERNAL public
//
SoNodeList * 
SoV1AppearanceKit::getUnusedTextureXfs()
//
////////////////////////////////////////////////////////////////////////
{
    return unusedTextureXfs;
}
