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
 |	SbLineProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/errors/SoDebugError.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

SbLineProjector::SbLineProjector()
{
    setLine(SbLine(SbVec3f(0,0,0), SbVec3f(0,1,0)));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns an instance that is a copy of this instance. The caller
//    is responsible for deleting the copy when done.
//
// Use: public, virtual
//

SbProjector *
SbLineProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbLineProjector *newProjector = new SbLineProjector;

    (*newProjector) = (*this);

    return newProjector;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//  Misc set routines.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

void
SbLineProjector::setLine(const SbLine &l)
{
    line = l;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Project onto this projector.
//
//  Originally this method created a working line by projecting the input
//  point into the scene, then transforming that line into workspace.
//  Afterwards, the point on the projector's line closest to the workingLine
//  was used as the result.
//
//  This technique provides good results when the mouse is on the line, 
//  but when it leaves the line, the closest point is not optimal.
//
//  The new method first transforms the projector's line into screen space,
//  then finds the nearest point on the screenSpaceLine to the input point.
//  It then uses this new screen space line and then performs the projection
//  as it used to.  
//
//  The result is that the screen point which we project into the scene
//  is now guaranteed to lie on the working line.
//
//  In short, first we find the closest point in screen space, then
//  we project that point back into work space.
//
// Use: public


SbVec3f
SbLineProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    // Convert two line points to world space
	SbLine worldLine;
	workingToWorld.multLineMatrix( line, worldLine );
	SbVec3f wldPt1 = worldLine.getPosition();
	SbVec3f wldDir = worldLine.getDirection();
	SbVec3f wldPt2 = wldPt1 + wldDir;

    // Convert two line points to normalized screen space.
	SbVec3f nrmScnPt1, nrmScnPt2;
	viewVol.projectToScreen( wldPt1, nrmScnPt1 );
	viewVol.projectToScreen( wldPt2, nrmScnPt2 );

    // Convert two line points and input point 
    // to viewPlane space, a screen space that's got view plane's aspect ratio:
	float vvW  = (viewVol.getWidth()  == 0.0) ? 1 : viewVol.getWidth();
	float vvH  = (viewVol.getHeight() == 0.0) ? 1 : viewVol.getHeight();
	SbVec3f     vpPt1( nrmScnPt1[0] * vvW, nrmScnPt1[1] * vvH, 0);
	SbVec3f     vpPt2( nrmScnPt2[0] * vvW, nrmScnPt2[1] * vvH, 0);
	SbVec3f vpInPoint(     point[0] * vvW,     point[1] * vvH, 0);

    // Create the viewPlaneLine -- our line expressed in viewPlane space:
        SbLine  viewPlaneLine( vpPt1, vpPt2 );

    // In viewplane space, find the closest point on our line to the cursor.
	SbVec3f vpClosestPt = viewPlaneLine.getClosestPoint( vpInPoint );
	vpClosestPt.setValue( vpClosestPt[0], vpClosestPt[1], 0 );

    // If we've got a perspective view, we may need to clamp the point we
    // choose so that it's not too close to the vanishing point.
    // Otherwise we'll just use our vpClosestPt

	SbVec3f vpClampedPt = vpClosestPt;

	if ( viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE ) {

	// Find the vanishing point of our line in viewPlane space: 
	    // Convert the direction of our line from world space into space 
	    // after the affine matrix (i.e. just before the projection matrix)
	    SbMatrix vvAffine, vvProj;
	    viewVol.getMatrices( vvAffine, vvProj );
	    SbVec3f postAffineDir;
	    vvAffine.multDirMatrix( wldDir, postAffineDir ); 

	  // If the direction of the line is parallel to the view plane,
	  // then the z component of postAffineDir is 0.
	  // In this case, we will not need to clamp our point and moreover,
	  // if we try we'll wind up dividing by zero pretty soon.
	  if ( postAffineDir[2] != 0.0 ) {

	    // If we send a line out from (0,0,0) into the viewVolume towards 
	    // postAffineDir, it will vanish at the same point as any other line
	    // parallel to this direction.  Also, all points along this line 
	    // will project to the same point on the near (or far) plane.
	    // So a line connecting (0,0,0) and the point at postAffineDir will
	    // intersect the near plane at the vanishing point.  Transforming 
	    // any point on this line by vvProj will yield the same x,y result 
	    // and the z component will vary with depth.
	    // So multiply the postAffineDir as a vector through the projection
	    // matrix and use the x,y for the vanishing point.
	    SbVec3f projVanish;
	    vvProj.multVecMatrix( postAffineDir, projVanish );

	    // Convert from [-1,1] range to [0,1] range for normalized coords.
	    SbVec3f nrmScnVanish;
	    nrmScnVanish[0] = (1.0 + projVanish[0]) * 0.5;
	    nrmScnVanish[1] = (1.0 + projVanish[1]) * 0.5;

	    // Finally, get the vanishing point in viewPlane coords:
	    SbVec3f vpVanish( nrmScnVanish[0] * vvW, nrmScnVanish[1] * vvH, 0 );

#if 0
	    // Check that the vanishing point is correct:
	    // Project nrmScnVanish on the plane to see if it goes along wldDir:
	    SbVec2f nrmScnVanish2( nrmScnVanish[0], nrmScnVanish[1] );
	    SbLine vanishWorldLine;
	    viewVol.projectPointToLine( nrmScnVanish2, vanishWorldLine );
	    SbVec3f test = vanishWorldLine.getDirection();
	    fprintf(stderr,"wldDir = %f %f %f\n",wldDir[0],wldDir[1],wldDir[2]);
	    fprintf(stderr,"checkDir = %f %f %f\n", test[0], test[1],test[2]);
#endif

	// The points vpPt1 and vpPt2 define the line in viewPlane space.
	// We can't go on the other side of the vanishing point from these 
	// defining points in screen space or the point will be undefined when 
	// we cast it into world space.
	// So clamp our selected point to lie on vpPt1's side of the vanishing 
	// point.  Since points near the vanishing point will also be incredibly
	// far away, introduce an (arbitrary) metric, VANISH_DELTA.
	// Our selection must be more than VANISH_DELTA times the average of
	// viewVolumeHeight and viewVolumeWidth from the vanishing point.
#define VANISH_DELTA .01
	    float vanishSafetyDist = VANISH_DELTA * .5 * (vvW + vvH);
#undef VANISH_DELTA
	    // Make pt0, the point from which we measure distances along vpLine.
	    // It will be one extra unit away from vpVanish than safetyDist
	    SbVec3f pt0 = viewPlaneLine.getPosition();
	    pt0.setValue( pt0[0], pt0[1], 0 );
	    SbVec3f pt0ToVanishDir = vpVanish - pt0;
	    pt0ToVanishDir.normalize();
	    float   pt0ToVanishDist = vanishSafetyDist + 1.0;
	    pt0 = vpVanish - pt0ToVanishDist * pt0ToVanishDir;

	    // Get vector and dist from pt0 to vpClosestPt
	    SbVec3f pt0ToClosest = vpClosestPt - pt0;
	    float   pt0ToClosestDist = pt0ToClosest.length();

	    // If vpClosestPt is too far from pt0, clamp it:
	    float   clampDist = pt0ToVanishDist - vanishSafetyDist;
	    if (    (pt0ToClosestDist > clampDist) 
		 && (pt0ToClosest.dot(pt0ToVanishDir) > 0.0) ) {
		vpClampedPt = pt0 + clampDist * pt0ToVanishDir;
	    }
	  }
	}

    // Convert result back into normalized screen space:
	SbVec2f nrmScnClampedPt( vpClampedPt[0] / vvW, vpClampedPt[1] / vvH);

    // Create a line in working space by projecting our point into the scene:
	SbVec3f result, whoCares;
	SbLine workingLine = getWorkingLine( nrmScnClampedPt );

    // Find point on the projector line closest to workingLine
	if (! line.getClosestPoints(workingLine, result, whoCares)) {
#ifdef DEBUG
	    SoDebugError::post("SbLineProjector::project",
			       "Couldn't get closest point");
#endif
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Get the vector based on one or two mouse positions.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

SbVec3f
SbLineProjector::getVector(const SbVec2f &m1, const SbVec2f &m2)
{
    SbVec3f p1 = project(m1);
    SbVec3f p2 = project(m2);
    
    lastPoint = p2;
    return p2 - p1;
}

SbVec3f
SbLineProjector::getVector(const SbVec2f &m)
{
    SbVec3f p = project(m);
    SbVec3f result = p - lastPoint;
    lastPoint = p;
    return result;
}

