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
 |	SoV1LightModel
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1LightModel.h"
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoLightModel.h>

SO_NODE_SOURCE(SoV1LightModel);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1LightModel::SoV1LightModel()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1LightModel);

    SO_NODE_ADD_FIELD(model,		(PHONG));
    SO_NODE_ADD_FIELD(nearColor,	(1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(farColor,		(0.0, 0.0, 0.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Model, PHONG);
    SO_NODE_DEFINE_ENUM_VALUE(Model, BASE_COLOR);
    SO_NODE_DEFINE_ENUM_VALUE(Model, DEPTH);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(model, Model);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1LightModel::~SoV1LightModel()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoLightModel.
//
// Use: private

SoNode *
SoV1LightModel::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoLightModel *result;

    Model mod = (Model) model.getValue();

    // Model "DEPTH" no longer exists. If it is used, we add an
    // SoEnvironment node with fog turned on to simulate depth
    // cueing. But since we can't turn this off properly, we print
    // a warning in the debugging version

    if (mod == DEPTH) {

	SoEnvironment	*env = SO_UPGRADER_CREATE_NEW(SoEnvironment);

	env->fogType  = SoEnvironment::HAZE;
	env->fogColor = farColor.getValue();

#ifdef DEBUG
	SoDebugError::postWarning("SoV1LightModel::createNewNode",
				  "converting LightModel depth cueing "
				  "into an SoEnvironment node with fog");
#endif /* DEBUG */

	return env;
    }

    // Regular light model is ok as is
    result = SO_UPGRADER_CREATE_NEW(SoLightModel);

    if (! model.isDefault()) {
	SO_UPGRADER_COPY_FIELD(model, result);
    }

    return result;
}

