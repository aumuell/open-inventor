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
 |      SoV1GroupKit
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1GroupKit.h>
#include <Inventor/misc/upgraders/SoV1AppearanceKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodekits/SoSeparatorKit.h>

// Define the required type id and name inquiry methods
SO_NODE_SOURCE(SoV1GroupKit);

// Define the required catalog inquiry methods
SO_V1_SUBKIT_CATALOG_VARS(SoV1GroupKit);
SO_V1_SUBKIT_CATALOG_METHODS(SoV1GroupKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1GroupKit::SoV1GroupKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1GroupKit);

    if(SO_NODE_IS_FIRST_INSTANCE() ) {

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use ...INHERIT_CATALOG once, and put it first.
    // Then, use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.
    SO_V1_SUBKIT_INHERIT_CATALOG(SoV1GroupKit, SoV1BaseKit );

    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("callbackList", SoSeparator,
				       "this", "", SoCallback, TRUE );
    SO_V1_SUBKIT_ADD_LIST_ITEM_TYPE("callbackList", SoEventCallback );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("topSeparator", SoSeparator,
				    "this", "", FALSE );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("pickStyle", SoPickStyle,
				       "topSeparator", "", TRUE );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("appearance",     SoAppearanceKit,
				    "topSeparator", "", TRUE );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("transform",      SoTransform,
				    "topSeparator", "", TRUE );

    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("texture2Transform", SoTexture2Transform,
				    "topSeparator", "", TRUE );

    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("childList", SoSeparator,
				       "topSeparator", "", SoSeparatorKit, TRUE );

    }

    
    

    createNodekitPartsList();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoV1GroupKit::~SoV1GroupKit()
//
////////////////////////////////////////////////////////////////////////
{
}
SoNode *
SoV1GroupKit::createNewNode()
{
    return new SoSeparatorKit;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method to setUp the new node.  
//
//     Overload this function. After SoV1BaseKit is done, see if there
//     is information in the nodelist
//     SoV1AppearanceKit::unusedTextureXfs.
//     If so, move this into our "texture2Transform" part.
//     Need to do this because Inventor 1.0 texture had fields for 
//     texture transforms, but new one does not. So SoV1AppearanceKit just
//     stores this info. We retrieve it and make use of it.
//
// Use: public

void
SoV1GroupKit::setUpNewNode(SoNode *newNode)
//
{
    SoV1BaseKit::setUpNewNode(newNode);

    SoSeparatorKit *newKit = (SoSeparatorKit *) newNode;

    // Return if newNode doesn't have an "appearance":
	SoNode *appKit = newKit->getPart("appearance", FALSE );
	if ( appKit == NULL)
	    return;

    // Return if the appKit didn't have unused texture2Transform fields:
	SoNodeList *kitList = SoV1AppearanceKit::getKitsWithUnusedTextureXfs();
	int listInd = kitList->find(appKit);
	if (listInd < 0)
	    return;

    // Get the SoTexture2Transform containing the info unused by our appearance
	SoNodeList *texXfList = SoV1AppearanceKit::getUnusedTextureXfs();
	SoTexture2Transform *appKitXf 
	    = (SoTexture2Transform *) (*texXfList)[listInd];

	if (appKitXf == NULL)
	    return;

    // Okay, there's a texture2Transform that got made when the appearanceKit
    // was read.  Let's install it as the texture2Transform part of newKit:

    // First, get the "texture2Transform" from newKit:
	SoTexture2Transform *newKitXf
	    = SO_GET_PART(newKit,"texture2Transform", SoTexture2Transform );

    // Determine if we're gonna stomp over an already set value in newKitXf:
	SbBool stompTranslate = FALSE;
	SbBool stompRotation = FALSE;
	SbBool stompScale = FALSE;
	SbBool stompCenter = FALSE;
	if (     appKitXf->translation.isDefault() == FALSE
	     &&  newKitXf->translation.isDefault() == FALSE )
		stompTranslate = TRUE;
	if (     appKitXf->rotation.isDefault() == FALSE
	     &&  newKitXf->rotation.isDefault() == FALSE )
		stompRotation = TRUE;
	if (     appKitXf->scaleFactor.isDefault() == FALSE
	     &&  newKitXf->scaleFactor.isDefault() == FALSE )
		stompScale = TRUE;
	if (     appKitXf->center.isDefault() == FALSE
	     &&  newKitXf->center.isDefault() == FALSE )
		stompCenter = TRUE;

    // Print a warning if we need to:
	if ( stompTranslate || stompRotation || stompScale || stompCenter ) {
    SoDebugError::postWarning("SoV1GroupKit::interpretCustomData",
    "texture2 contains texture transform fields. This kit also has a texture2Transform. The following fields  in texture2Transform will be overwritten:");
		if (stompTranslate)
    SoDebugError::postWarning("SoV1GroupKit::interpretCustomData", "translation");

		if (stompRotation)
    SoDebugError::postWarning("SoV1GroupKit::interpretCustomData", "rotation");

		if (stompScale)
    SoDebugError::postWarning("SoV1GroupKit::interpretCustomData", "scaleFactor");
		if (stompCenter)
    SoDebugError::postWarning("SoV1GroupKit::interpretCustomData", "center");
	}

    // Set the values in newKitXf
	if ( appKitXf->translation.isDefault() == FALSE )
	    newKitXf->translation = appKitXf->translation;
	if ( appKitXf->rotation.isDefault() == FALSE )
	    newKitXf->rotation = appKitXf->rotation; 
	if ( appKitXf->scaleFactor.isDefault() == FALSE )
	    newKitXf->scaleFactor = appKitXf->scaleFactor; 
	if ( appKitXf->center.isDefault() == FALSE )
	    newKitXf->center = appKitXf->center; 
}
