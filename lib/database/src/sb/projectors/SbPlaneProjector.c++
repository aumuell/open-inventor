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
 |	SbPlaneProjector
 |
 |   Author(s)		: Howard Look, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbPlaneProjector.h>
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

SbPlaneProjector::SbPlaneProjector(SbBool orient)
{
    orientToEye = FALSE;
    setPlane(SbPlane(SbVec3f(0,0,1), 0));
    setOrientToEye(orient);
}

SbPlaneProjector::SbPlaneProjector(
    const SbPlane &p,
    SbBool orient)
{
    orientToEye = FALSE;
    setPlane(p);
    setOrientToEye(orient);
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
SbPlaneProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbPlaneProjector *newProjector = new SbPlaneProjector;

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
SbPlaneProjector::setPlane(const SbPlane &p)
{
    plane = nonOrientPlane = p;
    needSetup = TRUE;
}

void
SbPlaneProjector::setOrientToEye(SbBool b)
{
    if (orientToEye != b) {
	orientToEye = b;
	needSetup = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Sets up the plane orientation, if necessary.
//
// Use: public

void
SbPlaneProjector::setupPlane()
//
////////////////////////////////////////////////////////////////////////
{
    if (orientToEye) {

	// Find the point that the old plane passes through that
	// is closest to the origin, and make the new plane pass
	// through that same point.
	SbVec3f planePoint = plane.getDistanceFromOrigin() *
			     plane.getNormal();

	SbVec3f newPlaneDir;
	// Find the normal for the new plane that points towards
	// the eye.
	if (viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE) {
	    // find the projection point in working space coords
	    SbVec3f workingProjPoint;
	    worldToWorking.multVecMatrix(
		viewVol.getProjectionPoint(), workingProjPoint);
	
	    newPlaneDir = workingProjPoint - planePoint;
	}
	else {
	    // Use the projection direction in an orthographic
	    // view vol
	    worldToWorking.multDirMatrix(viewVol.zVector(), newPlaneDir);
	}	 
	newPlaneDir.normalize();

	plane = SbPlane(newPlaneDir, planePoint);
    }
    else {
	// Reset the plane to be non-EyeOriented.
	plane = nonOrientPlane;
    }

    needSetup = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Project onto this projector.
//
//  Originally this method created workingLine by projecting the input
//  point into the scene and transforming it into workspace.
//  The workingLine was intersected with the projector plane to
//  get the result.
//
//  This technique provides good results when the cursor lies over the plane.
//  However, planes such as the ground plane vanish to a horizon line on the
//  view plane.  If the mouse is on the wrong side of the horizon line, it does
//  not intersect the projector plane.
//
//  In this case we need to find the closest point on the horizon line to 
//  the cursor and use that point instead.
//
//  In practical application, points on the horizon are infinitely far away
//  so the selected point will actually have to be some small distance
//  away from the horizon line.
//
//  Note that the horizon line is a line in the view plane and finding this
//  line is non-trivial.  Once the horizon line is found, we work in 
//  viewPlane space to find the closest point to the cursor.
//
// Use: public


SbVec3f
SbPlaneProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
	SbVec3f result;

    // Set up the plane if necessary
	if (needSetup)
	    setupPlane();

    // Start by assuming that we will have a succussful intersection using
    // the normalized screen point we were given:
	SbVec2f nrmScnClampedPt = point;

    // NOTE: when transforming normals you have to use the inverse
    // transpose or the normals will scale incorrectly.

    // Transform the plane from work space to world space.
	SbVec3f workNormal = plane.getNormal();
	SbVec3f workPlnPnt = workNormal * plane.getDistanceFromOrigin();

	SbVec3f wldNormal, wldPlnPnt;
	SbMatrix inverseTranspose = workingToWorld.inverse().transpose();
	inverseTranspose.multDirMatrix( workNormal, wldNormal );
	workingToWorld.multVecMatrix( workPlnPnt, wldPlnPnt );

    // Transform the plane from world space into space 
    // after the affine matrix (i.e. just before the projection matrix)
	SbMatrix vvAffine, vvProj;
	viewVol.getMatrices( vvAffine, vvProj );
	inverseTranspose = vvAffine.inverse().transpose();
	SbVec3f postAffineNormal, postAffinePlnPnt;
	inverseTranspose.multDirMatrix( wldNormal, postAffineNormal ); 
	postAffineNormal.normalize();
	vvAffine.multVecMatrix( wldPlnPnt, postAffinePlnPnt );

    // If now the plane is edge-on to the eye, there's no way we can give a 
    // good answer.  So just return the plane's origin and bail out:
    // The eye looks in the -z direction in postAffine space.
	SbVec3f postAffineEyeDir( 0, 0, -1 );
	// First test: see if plane normal is perpendicular to projection dir.
	if ( postAffineNormal.dot( postAffineEyeDir ) == 0.0 ) {
	    SbBool maybeOk = TRUE;

	    if ( viewVol.getProjectionType() == SbViewVolume::ORTHOGRAPHIC ) {
		// If orthographic, then this will always be an impossible
		// plane to project into:
		maybeOk = FALSE;
	    }
	    else {
		// Perpsective view: test to see if (0,0,0) lies within the 
		// plane. If so, then we are absolutely edge on.
		if ( postAffineNormal.dot( postAffinePlnPnt ) == 0.0 )
		    maybeOk = FALSE;
	    }

	    if ( ! maybeOk ) {
#ifdef DEBUG
		SoDebugError::post("SbPlaneProjector::project",
				   "Plane is edge-on to view.",
				   "Returning the plane origin");
#endif
		result = plane.getNormal() * plane.getDistanceFromOrigin();
		return result;
	    }
	}

    // If at this point the plane is flat against the view plane, all 
    // cursor positions will yield a valid intersection with the plane.
	SbBool isFlatAgainstViewPlane = FALSE;
	if ( fabs( postAffineNormal.dot( postAffineEyeDir )) == 1.0 ) 
	    isFlatAgainstViewPlane = TRUE;

    // It is important that our postAffinePlnPnt be in front of the eye.
    // Otherwise it will project onto the viewplane incorrectly.
    // For the remaining tasks, it is not important that it be the exact same 
    // point in space, just that it lies in the plane.  So check where it is 
    // and move it if need be.
	if ( (!isFlatAgainstViewPlane) && postAffinePlnPnt[2] >= 0.0 ) {
	    // Get a direction lying within the plane that is also within the
	    // viewplane:
	    SbVec3f dirInViewPlane = postAffineNormal.cross( SbVec3f( 0,0,-1) );
	    // The second cross product give direction within plane and also
	    // perpendicular to view plane.
	    SbVec3f dirInMyPlane   = dirInViewPlane.cross( postAffineNormal );

#ifdef DEBUG
	    if ( dirInMyPlane[2] == 0.0 ) {
		SoDebugError::post("SbPlaneProjector::project",
				   "Bad value for direction in plane");
	    }
#endif
	    // Find out how far we need to travel along the direction
	    // before we hit the z=0 plane:
	    // postAffinePlnPnt[2] + distToGo * dirInMyPlane[2] = 0
	    float distToGo = (-1 * (postAffinePlnPnt[2] )) / dirInMyPlane[2];
	    // Go a bit further to insure negative z
	    postAffinePlnPnt = postAffinePlnPnt + 1.1 * distToGo * dirInMyPlane;
	}

    // If we've got a perspective view, we may need to clamp the point we
    // choose so that it's not too close to the horizon line.
    // Otherwise we'll just use our nrmScnClampedPt;

	if ( viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE 
	    && !isFlatAgainstViewPlane ) {

	    float vvW  = (viewVol.getWidth()  == 0.0) ? 1 : viewVol.getWidth();
	    float vvH  = (viewVol.getHeight() == 0.0) ? 1 : viewVol.getHeight();

	    // To find horizon line, get two directions within the plane,
	    // find their vanishing points and connect them.

	    // Get two different directions within the plane:
	    // Note that since postAffineEyeDir & normal are not parallel
	    // this is always a valid calculation.
	    // Since cross product is always perp to both vecs, this dir will
	    // lie in the viewplane:
	    SbVec3f dirInViewPlane = postAffineEyeDir.cross( postAffineNormal );
	    SbVec3f postAffineDir1 = dirInViewPlane.cross( postAffineNormal );
	    SbVec3f postAffineDir2 = 0.5 * (dirInViewPlane + postAffineDir1 );
	    postAffineDir2.normalize();

	    // Get their vanishing points in normalized screen space.
	    // Send a line out from (0,0,0) into the viewVolume towards 
	    // each direction. Each vanishes at the same point as any other line
	    // parallel to it.  Also, all points along that line project
	    // to the same point on the near (or far) plane. So a line 
	    // connecting (0,0,0) and the point at postAffineDir1 (or 2) will
	    // intersect the near plane at its vanishing point.  Transforming 
	    // any point on this line by vvProj will yield the same x,y result 
	    // and the z component will vary with depth.
	    // So multiply the postAffineDirs as vectors through the projection
	    // matrix and use the x,y for the vanishing points.
	    SbVec3f projVanish1, projVanish2, projPlnPnt;
	    vvProj.multVecMatrix( postAffineDir1,   projVanish1 );
	    vvProj.multVecMatrix( postAffineDir2,   projVanish2 );
	    vvProj.multVecMatrix( postAffinePlnPnt, projPlnPnt );

	    // Transform from [-1,1] range to [0,1] range for normalized coords.
	    SbVec3f nrmScnVanish1, nrmScnVanish2, nrmScnPlnPnt;
	    nrmScnVanish1[0] = (1.0 + projVanish1[0]) * 0.5;
	    nrmScnVanish1[1] = (1.0 + projVanish1[1]) * 0.5;
	    nrmScnVanish2[0] = (1.0 + projVanish2[0]) * 0.5;
	    nrmScnVanish2[1] = (1.0 + projVanish2[1]) * 0.5;
	    nrmScnPlnPnt[0]  = (1.0 + projPlnPnt[0])  * 0.5;
	    nrmScnPlnPnt[1]  = (1.0 + projPlnPnt[1])  * 0.5;

	    // Finally, get the vanishing points in viewPlane coords:
	    SbVec3f vpVanish1(nrmScnVanish1[0] * vvW,nrmScnVanish1[1] * vvH, 0);
	    SbVec3f vpVanish2(nrmScnVanish2[0] * vvW,nrmScnVanish2[1] * vvH, 0);
	    SbVec3f  vpPlnPnt( nrmScnPlnPnt[0] * vvW, nrmScnPlnPnt[1] * vvH, 0);

	    // Connect them to form the horizon:
	    SbLine horizon( vpVanish1, vpVanish2 );

	    // Is the input point on the opposite side of the horizon from
	    // our plane?  Or on the same side but very close?
	    // Introduce a new metric, lineToLine.
	    // This is the distance on the view plane between the horizon line
	    // and the line where our projection plane slices through the
	    // viewplane.
	    // Introduce an (arbitrary) amount, VANISH_DELTA.
	    // If too near the horizon, select a point closer than the horizon 
	    // by an amount of VANISH_DELTA times lineToLine.

	    // First, determine sliceLine, the line where the projectionPlane
	    // slices through the viewPlane:
// BEGIN STUFF FROM GRAPHICS GEMS 3, p.235 for intersecting 2 planes:
	    SbVec3f normM = postAffineNormal;
	    float   distI = -1 * normM.dot( postAffinePlnPnt );
	    SbVec3f normN = SbVec3f( 0, 0, -1 );
	    float   distJ = -1 * viewVol.getNearDist();
	    SbVec3f normL = normM.cross( normN );
	    int indexU, indexV, indexW;
	    if (   fabs(normL[0]) > fabs(normL[1]) 
		&& fabs(normL[0]) > fabs(normL[2]) ) {
		indexW = 0; indexU = 1; indexV = 2;
	    }
	    else if ( fabs(normL[1]) > fabs(normL[2]) ) {
		indexW = 1; indexU = 0; indexV = 2;
	    }
	    else {
		indexW = 2; indexU = 0; indexV = 1;
	    }
	    SbVec3f slicePoint1, slicePoint2;
	    float denom =   (normM[indexU] * normN[indexV]) 
			  + (normM[indexV] * normN[indexU]);
	    slicePoint1[indexU] =
		((normM[indexV] * distJ) - (normN[indexV] * distI)) / denom;
	    slicePoint1[indexV] =
		((normN[indexU] * distI) - (normM[indexU] * distJ)) / denom;
	    slicePoint1[indexW] = 0.0;
	    normL.normalize();
// END STUFF FROM GRAPHICS GEMS 3, p.235 for intersecting 2 planes:
	    // Convert slicePoint1 to viewPlane coords:
	    SbVec3f vpSlicePoint;
	    vvProj.multVecMatrix( slicePoint1,   vpSlicePoint );
	    vpSlicePoint[0] = ((1.0 + vpSlicePoint[0]) * 0.5) * vvW;
	    vpSlicePoint[1] = ((1.0 + vpSlicePoint[1]) * 0.5) * vvH;
	    vpSlicePoint[2] = 0.0;
	    // Now calculate lineToLine, the distance between vpSlicePoint and
	    // the horizon:
	    SbVec3f nearToSlicePoint = horizon.getClosestPoint( vpSlicePoint );
	    float lineToLine = (vpSlicePoint - nearToSlicePoint).length();
	    // If lineToLine is as big as the average of the viewplane width
	    // and height, then the plane is pretty flat on. Reduce lineToLine
	    // to be that size.
	    float sizeAverage = 0.5 * (vvW + vvH);
	    if (lineToLine > sizeAverage)
	        lineToLine = sizeAverage;
#define VANISH_DELTA .01
	    float vanishSafetyDist = VANISH_DELTA * lineToLine;
#undef VANISH_DELTA
	    SbVec3f vpInPoint(     point[0] * vvW,     point[1] * vvH, 0);
	    SbVec3f nearToInPoint = horizon.getClosestPoint( vpInPoint );
	    SbVec3f nearToPlnPnt  = horizon.getClosestPoint( vpPlnPnt );
	    SbVec3f vec1 = vpInPoint - nearToInPoint;
	    SbVec3f vec2 = vpPlnPnt - nearToPlnPnt;
	    float vec1Length = vec1.length();
	    if ( vec1.dot( vec2 ) < 0.0 || vec1Length < vanishSafetyDist ) {
		SbVec3f mvDir = vec2;
		mvDir.normalize();
		SbVec3f vpClampedPt = nearToInPoint + vanishSafetyDist * mvDir;
		nrmScnClampedPt.setValue( vpClampedPt[0] / vvW, 
					  vpClampedPt[1] / vvH );
	    }

	}

    // Project our point into a line in working space 
	SbLine workingLine = getWorkingLine( nrmScnClampedPt );

    // Intersect that line with the working space plane
	if (! plane.intersect(workingLine, result)) {
#ifdef DEBUG
	    SoDebugError::post("SbPlaneProjector::project",
			       "Couldn't intersect with plane");
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
SbPlaneProjector::getVector(const SbVec2f &m1, const SbVec2f &m2)
{
    SbVec3f p1 = project(m1);
    SbVec3f p2 = project(m2);
    
    lastPoint = p2;
    return p2 - p1;
}

SbVec3f
SbPlaneProjector::getVector(const SbVec2f &m)
{
    SbVec3f p = project(m);
    SbVec3f result = p - lastPoint;
    lastPoint = p;
    return result;
}
