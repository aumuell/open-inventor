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
 |	SoV1Environment
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1Environment.h"
#include <Inventor/nodes/SoEnvironment.h>

SO_NODE_SOURCE(SoV1Environment);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1Environment::SoV1Environment()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1Environment);

    SO_NODE_ADD_FIELD(ambientIntensity,	(0.2));
    SO_NODE_ADD_FIELD(ambientColor,	(1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(attenuation,	(0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(fogType,		(NONE));
    SO_NODE_ADD_FIELD(fogComputed,	(PER_VERTEX));
    SO_NODE_ADD_FIELD(fogColor,		(1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(fogDensity,	(0.0));
    SO_NODE_ADD_FIELD(fogNearDistance,	(1.0));
    SO_NODE_ADD_FIELD(fogFarDistance,	(10.0));

    // Set up static info for enumerated type fields
    SO_NODE_DEFINE_ENUM_VALUE(Type, NONE);
    SO_NODE_DEFINE_ENUM_VALUE(Type, LINEAR);
    SO_NODE_DEFINE_ENUM_VALUE(Type, EXPONENTIAL);
    SO_NODE_DEFINE_ENUM_VALUE(Type, EXPONENTIAL_SQUARED);
    SO_NODE_DEFINE_ENUM_VALUE(Computed, PER_VERTEX);
    SO_NODE_DEFINE_ENUM_VALUE(Computed, PER_PIXEL);

    // Set up info in enumerated type fields
    SO_NODE_SET_SF_ENUM_TYPE(fogType, Type);
    SO_NODE_SET_SF_ENUM_TYPE(fogComputed, Computed);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1Environment::~SoV1Environment()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoEnvironment.
//
// Use: private

SoNode *
SoV1Environment::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoEnvironment *result = SO_UPGRADER_CREATE_NEW(SoEnvironment);

    // Convert from old enum to new enum
    if (! fogType.isDefault()) {
	switch ((Type) fogType.getValue()) {
	  case NONE:
	    result->fogType = SoEnvironment::NONE;
	    break;
	  case LINEAR:
	    result->fogType = SoEnvironment::HAZE;
	    break;
	  case EXPONENTIAL:
	    result->fogType = SoEnvironment::SMOKE;
	    break;
	  case EXPONENTIAL_SQUARED:
	    result->fogType = SoEnvironment::FOG;
	    break;
	}
    }

    // fogComputed is ignored totally

    // Set other fields as is
    SO_UPGRADER_COPY_FIELD(ambientIntensity, result);
    SO_UPGRADER_COPY_FIELD(ambientColor, result);
    SO_UPGRADER_COPY_FIELD(attenuation, result);
    SO_UPGRADER_COPY_FIELD(fogColor, result);

    // Convert density to visibility
    if (! fogDensity.isDefault()) {
	if (result->fogType.getValue() == SoEnvironment::SMOKE)
	    result->fogVisibility = 2.0 / fogDensity.getValue();
	else
	    result->fogVisibility = 4.0 / fogDensity.getValue();
	
	if (fogDensity.isIgnored())
	    result->fogVisibility.setIgnored(TRUE);
    }	

    // Near and far distances are ignored, since they now (typically)
    // use the camera planes

    return result;
}
