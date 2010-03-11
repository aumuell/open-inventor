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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoEnvironmentV1
 |
 |   Author(s): Paul S. Strauss, Gavin Bell, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Util.h"
#include "SoEnvironmentV1.h"
#include <Inventor/nodes/SoEnvironment.h>

SO_NODE_SOURCE(SoEnvironmentV1);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoEnvironmentV1::SoEnvironmentV1()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoEnvironmentV1);

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

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoEnvironment class.
//
// Use: internal

void
SoEnvironmentV1::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoEnvironmentV1, "Environment", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy version 1.0 fields from the passed 2.0 node.
//
// Use: public
//
SoNode *
SoEnvironmentV1::downgrade(SoEnvironment *env2)
//
////////////////////////////////////////////////////////////////////////
{
    SoEnvironmentV1 *env1 = new SoEnvironmentV1;
    
    // Convert from 2.0 enum to 1.0 enum
    if (! env2->fogType.isDefault()) {
	switch ((Type) env2->fogType.getValue()) {
	  case SoEnvironment::NONE:
	    env1->fogType = NONE;
	    break;
	  case SoEnvironment::HAZE:
	    env1->fogType = LINEAR;
	    break;
	  case SoEnvironment::SMOKE:
	    env1->fogType = EXPONENTIAL;
	    break;
	  case SoEnvironment::FOG:
	    env1->fogType = EXPONENTIAL_SQUARED;
	    break;
	}
    }

    // fogComputed is ignored totally in 2.0

    // Set other fields as is
    COPY_FIELD(env1, env2, ambientIntensity);
    COPY_FIELD(env1, env2, ambientColor);
    COPY_FIELD(env1, env2, attenuation);
    COPY_FIELD(env1, env2, fogColor);

    // Convert visibility to density
    if (! env2->fogVisibility.isDefault()) {
	if (env2->fogType.getValue() == SoEnvironment::SMOKE)
	    env1->fogDensity = 2.0 / env2->fogVisibility.getValue();
	else
	    env1->fogDensity = 4.0 / env2->fogVisibility.getValue();
	
	if (env2->fogVisibility.isIgnored())
	    env1->fogDensity.setIgnored(TRUE);
    }
    
    // Near and far distances are ignored, since they now (typically)
    // use the camera planes
    
    return env1;
}
