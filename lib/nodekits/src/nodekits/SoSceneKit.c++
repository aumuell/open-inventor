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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoSceneKit
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodekits/SoSceneKit.h>
#include <Inventor/nodekits/SoCameraKit.h>
#include <Inventor/nodekits/SoLightKit.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

SO_KIT_SOURCE(SoSceneKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSceneKit::SoSceneKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoSceneKit);

    isBuiltIn = TRUE;

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.

    // this is a switch node, so the user can pick which camera to look 
    // through.
    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE, this, , FALSE);
    SO_KIT_ADD_CATALOG_LIST_ENTRY(cameraList, SoSwitch, TRUE,
					topSeparator, , SoCameraKit, TRUE );
    SO_KIT_ADD_CATALOG_LIST_ENTRY(lightList, SoGroup, TRUE,
					topSeparator, , SoLightKit, TRUE );
    SO_KIT_ADD_CATALOG_LIST_ENTRY(childList, SoGroup, TRUE,
					topSeparator, , SoShapeKit, TRUE );
    SO_KIT_ADD_LIST_ITEM_TYPE(childList, SoSeparatorKit );

    SO_KIT_INIT_INSTANCE();

    // Install a camera kit by default.
    setPart( "cameraList[0]", new SoCameraKit );

    setCameraNumber( 0 );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoSceneKit::~SoSceneKit()
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
SoSceneKit::getCameraNumber()
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *camContainer = getContainerNode( "cameraList" );

    if (        camContainer == NULL
         ||   !(camContainer->isOfType( SoSwitch::getClassTypeId() ) ) )
	return -1;

    SoSwitch *camSwitch = (SoSwitch *) camContainer;

    int answer = (int) camSwitch->whichChild.getValue();
    return answer;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//     sets the switch node in 'cameraList' to be the number given
//
// Use: public

void
SoSceneKit::setCameraNumber(int camNum )
//
////////////////////////////////////////////////////////////////////////
{

    SoNode *camContainer = getContainerNode( "cameraList" );

    if (        camContainer == NULL
         ||   !(camContainer->isOfType( SoSwitch::getClassTypeId() ) ) )
	return;

    SoSwitch *camSwitch = (SoSwitch *) camContainer;

    int 	numCams = camSwitch->getNumChildren();

    if ( camNum < 0 || (camNum >= 0 && camNum < numCams) ) {

	camSwitch->whichChild.setValue( camNum );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    overrides method in SoNode to return FALSE.
//
// Use: public
//
SbBool
SoSceneKit::affectsState() const
{
    return FALSE;
}
