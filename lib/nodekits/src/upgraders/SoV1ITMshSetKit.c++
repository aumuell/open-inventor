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
 |      SoV1IndexedTriangleMeshKit
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/misc/upgraders/SoV1IndexedTriangleMeshKit.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodekits/SoShapeKit.h>

// Define the required type id and name inquiry methods
SO_NODE_SOURCE(SoV1IndexedTriangleMeshKit);

// Define the required catalog inquiry methods
SO_V1_SUBKIT_CATALOG_VARS(SoV1IndexedTriangleMeshKit);
SO_V1_SUBKIT_CATALOG_METHODS(SoV1IndexedTriangleMeshKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1IndexedTriangleMeshKit::SoV1IndexedTriangleMeshKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1IndexedTriangleMeshKit);

    if(SO_NODE_IS_FIRST_INSTANCE() ) {

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use ...INHERIT_CATALOG once, and put it first.
    // Then, use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.
    SO_V1_SUBKIT_INHERIT_CATALOG(SoV1IndexedTriangleMeshKit, SoV1VertexShapeKit );
    SO_V1_SUBKIT_CHANGE_ENTRY_TYPE("shape", SoIndexedFaceSet, 
					 SoIndexedFaceSet );

    }

    
    

    createNodekitPartsList();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoV1IndexedTriangleMeshKit::~SoV1IndexedTriangleMeshKit()
//
////////////////////////////////////////////////////////////////////////
{
}

SoNode *
SoV1IndexedTriangleMeshKit::createNewNode()
{
    return new SoShapeKit;
}
