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
 |      SoShapeKit
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoShapeKit.h>

#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLinearProfile.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNurbsProfile.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoProfileCoordinate3.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTextureCoordinateFunction.h>
#include <Inventor/nodes/SoTextureCoordinateDefault.h>
#include <Inventor/nodes/SoTransform.h>


SO_KIT_SOURCE(SoShapeKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoShapeKit::SoShapeKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoShapeKit);

    isBuiltIn = TRUE;

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.

    // Binding nodes
    SO_KIT_ADD_CATALOG_ENTRY(materialBinding, SoMaterialBinding, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(normalBinding, SoNormalBinding, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(textureCoordinateBinding, 
	    SoTextureCoordinateBinding, TRUE, topSeparator, childList, TRUE );

    // Shapehints
    SO_KIT_ADD_CATALOG_ENTRY(shapeHints, SoShapeHints, TRUE,
				    topSeparator, childList, TRUE );

    // Coordinate and normal nodes
    SO_KIT_ADD_CATALOG_ENTRY(coordinate3, SoCoordinate3, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(coordinate4, SoCoordinate4, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(normal, SoNormal, TRUE, 
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(textureCoordinate2, SoTextureCoordinate2, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY( textureCoordinateFunction, 
		    SoTextureCoordinateFunction, SoTextureCoordinateDefault,
		    TRUE, topSeparator, ,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(profileCoordinate2, SoProfileCoordinate2, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(profileCoordinate3, SoProfileCoordinate3, TRUE,
				    topSeparator, childList, TRUE );
    SO_KIT_ADD_CATALOG_LIST_ENTRY(profileList, SoGroup, TRUE,
		    		    topSeparator, childList, SoProfile, TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(localTransform, SoTransform, TRUE,
				    topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(shapeSeparator, SoSeparator, TRUE,
				    topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(shape, SoShape,
			    SoCube, FALSE, shapeSeparator, , TRUE  );

    SO_KIT_INIT_INSTANCE();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoShapeKit::~SoShapeKit()
//
////////////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////////////
//
// Called by the SoBaseKit::write() method. Calls setDefault(TRUE)
// on the shapeSeparator. Note that this may be overriden later by basekit
// if, for example, topSeparator lies on a path that is being written out.
//
/////////////////////////////////////////////////////////////////////////
void
SoShapeKit::setDefaultOnNonWritingFields()
{
    shapeSeparator.setDefault(TRUE);

    // Call the base class...
    SoSeparatorKit::setDefaultOnNonWritingFields();
}
