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
 * Copyright (C) 1990-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV2MaterialIndex
 |
 |   Author(s)		: David mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV2MaterialIndex.h"
#include <Inventor/nodes/SoColorIndex.h>

SO_NODE_SOURCE(SoV2MaterialIndex);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV2MaterialIndex::SoV2MaterialIndex()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV2MaterialIndex);

    // Default values taken from the 2.0.1 tree
    SO_NODE_ADD_FIELD(ambientIndex,	(1));
    SO_NODE_ADD_FIELD(diffuseIndex,	(2));
    SO_NODE_ADD_FIELD(specularIndex,	(3));
    SO_NODE_ADD_FIELD(shininess,	(0.2));
    SO_NODE_ADD_FIELD(transparency,	(0.0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV2MaterialIndex::~SoV2MaterialIndex()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.1 SoColorIndex.
//
// Use: private

SoNode *
SoV2MaterialIndex::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoColorIndex *result = SO_UPGRADER_CREATE_NEW(SoColorIndex);

    //
    // MaterialIndex has gone away in 2.1.
    // Simply copy the diffuseIndex values to a new ColorIndex node.
    //

    SO_UPGRADER_COPY_FIELD2(diffuseIndex, index, result);

    return result;
}

