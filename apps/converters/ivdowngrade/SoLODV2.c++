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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Classes:
 |	SoLODV2
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <math.h>
#include <float.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include "SoLODV2.h"

SO_NODE_SOURCE(SoLODV2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoLODV2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLODV2, "LOD", SoLOD);

    // Tell the type system whenever it wants to create something
    // of type SoLOD to create a SoLODV2
    SoType::overrideType(
		 SoLOD::getClassTypeId(), &SoLODV2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoLODV2::SoLODV2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLODV2);
    SO_NODE_ADD_FIELD(alternateRep, (NULL));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoLODV2::~SoLODV2()
//
////////////////////////////////////////////////////////////////////////
{
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    write the alternate rep for this node
//
void
SoLODV2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {
	// Only do this once!
	if (alternateRep.isDefault()) {
	    SoLevelOfDetail *levelOfDetail = new SoLevelOfDetail;
	    alternateRep = levelOfDetail;

	    // All LOD children become LevelOfDetail children
	    int j;
	    for (j=0; j < getNumChildren(); j++)
		levelOfDetail->addChild(getChild(j));
    
	    // We don't need children here any more - they are in the alternateRep
	    removeAllChildren();
	    
	    // Convert the LOD ranges to LevelOfDetail screen areas
	    // We reverse the process of ivToVRML's convertLevelOfDetailToLOD():
	    
	    // Assuming a screen height and width of 1024 and a 45 degree field
	    // of view, the formula for converting a screenArea to a distance
	    // is:
	    //
	    //                       (1024 / 2) * worldBboxRadius
	    //     distance = ______________________________________
	    //
	    //                 tan( 45 / 2) * sqrt(screenArea / PI)
	    //
    
	    // So, converting distance to screenArea is:
	    //
	    //		(1024/2) * worldBboxRadius
	    //	f = --------------------------------	
	    //		tan( 45 / 2 ) * distance
	    //
	    //
	    //	screenArea = f^2 * PI
	    //
	    
	    const SoPath *path = action->getCurPath();
    
	    // get the matrix
	    SoGetMatrixAction matAction(SbViewportRegion(1024, 1024));
    
	    matAction.apply((SoPath *) path); // const cast away!
    
	    SbMatrix toWorld = matAction.getMatrix();
    
	    // transform the bbox to world space
	    SoGetBoundingBoxAction bba(SbViewportRegion(100, 100));
	    bba.apply(levelOfDetail);
	    SbBox3f bbox = bba.getBoundingBox();
    
	    bbox.transform(toWorld);
	    
	    float w,h,d;
	    bbox.getSize(w,h,d);
    
	    w *= 0.5;
	    h *= 0.5;
	    d *= 0.5;
    
	    float radius = sqrt(w*w + h*h + d*d);
    
	    // empirically, half the radius seems to work better
	    radius *= 0.5;
    
	    float distance, newScreenArea;
	    float tanHalf45 = tan((0.5 * 45.0 / 180.0) * M_PI);
	    
	    for (j=0; j < range.getNum(); j++) {
		distance = range[j];
		if (distance != 0.0) {
		    newScreenArea = 512.0 * radius / (tanHalf45 * distance);
		    newScreenArea = newScreenArea * newScreenArea * M_PI;
		} else {
		    newScreenArea = FLT_MAX;
		}
		levelOfDetail->screenArea.set1Value(j, newScreenArea);
	    }
	}
    }
    
    SoLOD::write(action);
}

