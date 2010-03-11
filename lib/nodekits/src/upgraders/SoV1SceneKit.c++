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
 |      SoV1SceneKit
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1SceneKit.h>
#include <Inventor/nodekits/SoCameraKit.h>
#include <Inventor/nodekits/SoLightKit.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodekits/SoSceneKit.h>

// Define the required type id and name inquiry methods
SO_NODE_SOURCE(SoV1SceneKit);

// Define the required catalog inquiry methods
SO_V1_SUBKIT_CATALOG_VARS(SoV1SceneKit);
SO_V1_SUBKIT_CATALOG_METHODS(SoV1SceneKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1SceneKit::SoV1SceneKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1SceneKit);

    if(SO_NODE_IS_FIRST_INSTANCE() ) {

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use ...INHERIT_CATALOG once, and put it first.
    // Then, use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.
    SO_V1_SUBKIT_INHERIT_CATALOG(SoV1SceneKit, SoV1BaseKit );

    // this is a switch node, so the user can pick which camera to look 
    // through.
    SO_V1_SUBKIT_ADD_CATALOG_ENTRY("topSeparator", SoSeparator, "this", "", FALSE);
    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("cameraList", SoSwitch,
					"topSeparator", "", SoCameraKit, TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("lightList", SoGroup,
					"topSeparator", "", SoLightKit, TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("childList", SoGroup,
				    "topSeparator", "", SoSeparatorKit, TRUE );
    SO_V1_SUBKIT_ADD_CATALOG_LIST_ENTRY("manipulatorList", SoSeparator,
				    "topSeparator", "", SoInteractionKit, TRUE );

    }

    
    

    createNodekitPartsList();
    if ( getNodekitCatalog() != NULL ) {
	setCameraNumber( 0 );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoV1SceneKit::~SoV1SceneKit()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     gets the child number of the camera in 'cameraList' that is currently
//     selected by the switch node.
//
// Use: public

int
SoV1SceneKit::getCameraNumber()
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup     *grp;
    SoSwitch    *sw;
    SbPList     eltTypes;

    grp = (SoGroup *) getAnyPart( "cameraList", SoSwitch::getClassTypeId(), TRUE, FALSE, FALSE );
    sw = (SoSwitch *) grp;
    int answer = (int) sw->whichChild.getValue();
    return answer;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//     sets the switch node in 'cameraList' to be the number given
//
// Use: public

void
SoV1SceneKit::setCameraNumber(int camNum )
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup	*grp;
    SoSwitch    *sw;
    SbPList     eltTypes;

    grp = (SoGroup *) getAnyPart("cameraList",SoSwitch::getClassTypeId(),TRUE, FALSE, FALSE );
    int 	numCams = grp->getNumChildren();
    if ( camNum == 0 || (camNum >= 0 && camNum < numCams) ) {
	sw = (SoSwitch *) grp;

	sw->whichChild.setValue( camNum );
    }
}
SoNode *
SoV1SceneKit::createNewNode()
{
    return new SoSceneKit;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    A virtual function that allows classes to treat parts that fail the
//    tests of 'tryToSetPartInNewNode'
//
// If tryToSetPartInNewNode fails, then this routine is called.
// It will fail for the parts:
// "manipulatorList"
// This routine will simply discard those parts with a warning, but
// no error.
//
// Use: public
//
SbBool 
SoV1SceneKit::dealWithUpgradedPart( SoBaseKit *newNode, SoNode *newPart, 
				    const SbName &newPartName )
//
////////////////////////////////////////////////////////////////////////
{
    // First, try to let base class handle it...
    if ( SoV1BaseKit::dealWithUpgradedPart( newNode, newPart, newPartName ) )
	return TRUE;

    // If the part name is "manipulatorList" just print
    // a warning, don't set the part, and return TRUE.
    if ( newPartName == "manpulatorList" ) {

	SoDebugError::postWarning("SoV1SceneKit::dealWithUpgradedPart",
	    "the input file contained a part named %s. This part no longer exists, so you will unfortunately have to lose it.", newPartName.getString() );
	return TRUE;
    }

    return FALSE;   // don't know how to do anything yet...
}
