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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV1Material
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1Material.h"
#include <Inventor/nodes/SoMaterial.h>

SO_NODE_SOURCE(SoV1Material);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1Material::SoV1Material()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1Material);

    SO_NODE_ADD_FIELD(ambientColor,	(0.2, 0.2, 0.2));
    SO_NODE_ADD_FIELD(diffuseColor,	(0.8, 0.8, 0.8));
    SO_NODE_ADD_FIELD(specularColor,	(0.0, 0.0, 0.0));
    SO_NODE_ADD_FIELD(emissiveColor,	(0.0, 0.0, 0.0));
    SO_NODE_ADD_FIELD(shininess,	(0.0));
    SO_NODE_ADD_FIELD(transparency,	(0.0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1Material::~SoV1Material()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoMaterial.
//
// Use: private

SoNode *
SoV1Material::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterial *result = SO_UPGRADER_CREATE_NEW(SoMaterial);

    //
    // OpenGL differs from IRIS GL in its treatment of the case where
    // shininess = 0. In IRIS GL, a shininess of 0 turned off all
    // specular highlights, whereas in OpenGL, a shininess of 0
    // disperses any specularity over the entire surface. To disable
    // specular highlights in OpenGL, you have to set the specular
    // color to (0,0,0).
    //
    // We decided the Inventor 2.0 material model should be the same
    // as OpenGL's. To avoid problems, we can simulate IRIS GL's
    // handling of shininess 0 by making sure the specular color for
    // any material with shininess 0 is (0,0,0). We also use the new
    // default shininess of 0.2 unless it is set otherwise.
    //

    if (shininess.isDefault() ||
	(shininess.getNum() == 1 && shininess[0] == 0.0)) {

	// If shininess was set to 0, leave specular color as default
	// value, but set shininess to 0.0, since someone wanted it
	// that way. Otherwise, leave them both as default values.
	if (! shininess.isDefault())
	    result->shininess = 0.0;

	// Check ignore flags
	if (shininess.isIgnored())
	    result->shininess.setIgnored(TRUE);
	if (specularColor.isIgnored())
	    result->specularColor.setIgnored(TRUE);
    }

    // Any other value of shininess is ok, so copy both into the new material
    else {
	SO_UPGRADER_COPY_FIELD(specularColor, result);
	SO_UPGRADER_COPY_FIELD(shininess,     result);
    }

    // Copy the other fields as is
    SO_UPGRADER_COPY_FIELD(ambientColor,  result);
    SO_UPGRADER_COPY_FIELD(diffuseColor,  result);
    SO_UPGRADER_COPY_FIELD(emissiveColor, result);
    SO_UPGRADER_COPY_FIELD(transparency,  result);

    return result;
}

