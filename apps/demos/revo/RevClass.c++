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

//
// Code for a Surface of Revolution class.  This is not really very
// robust or well-designed (or complete or...).
//

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoQuadMesh.h>

#include "RevClass.h"

//
// Utility function used to figure out the normal of a 2D line
// segment.
//
static inline void
figureNormal(const SbVec3f &p1, const SbVec3f &p2, SbVec3f &result)
{
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    result.setValue(-dy, dx, 0.0);
    result.normalize();
}

//
// This is used to figure out if the angle between the three given
// points is greater than the given angle (given as cos(angle)).  If
// it is, code further down will create a sharp (faceted) edge.
//
static int
angleGreater(const SbVec3f &p0, const SbVec3f &p1, const SbVec3f &p2,
	     double cosCreaseAngle)
{
    SbVec3f v1 = p0 - p1;
    SbVec3f v2 = p1 - p2;
    double cAngle;
    double l1 = v1.length();
    double l2 = v2.length();
    if (l1 < 0.001 || l2 < 0.001) cAngle = 0.0;
    else
    {
	v1 /= l1; v2 /= l2;
	cAngle = v1.dot(v2);
    }
    return cAngle < cosCreaseAngle;
}

//
// Given a set of 2D profile coordinates (the z-coordinate of this
// Vec3f field is ignored), this creates a surface of revolution
// (including normals).
// If passed NULL, this routine will use the last coordinates passed
// (this is used when the number of sides in the revolution is
// changed).
//
void
RevolutionSurface::createSurface(const SoMFVec3f *prof)
{
    static int p_alloc = 0;
    static SbVec3f *profile = NULL;
    static double cca;

    //
    // Ok, cheesy hack!
    // First time through, allocate space for profile points, and
    // calculate cosine of crease angle.
    //
    if (p_alloc == 0)
    {
	p_alloc = 2;
	profile = new SbVec3f[p_alloc*2];
	cca = cos(creaseAngle);
    }

    int nr, nc;
    int i, j;

    if (prof != NULL)
    {
	nc = NumSides;
	nr = 0;

	const SoMFVec3f &pc = (*prof);
	int pcn = pc.getNum();
	if (pcn*2 > p_alloc)
	{
	    // Allocate more space for profile points
	    p_alloc = pcn*4;
	    delete[] profile;
	    profile = new SbVec3f[p_alloc];
	}

	//
	// If the user drew from bottom to top, cool...
	//
	if (pcn > 0 && pc[0][1] < pc[pcn-1][1])
	{
	    for (j = 0; j < pcn; j++)
	    {
		profile[nr] = pc[j];
		++nr;
		// 
		// If the angle at an interior vertex is too big, we
		// create an extra vertex to get a sharp edge
		//
		if (j > 0 && j < pcn-1 &&
		    angleGreater(pc[j-1], pc[j], pc[j+1], cca))
		{
		    profile[nr] = pc[j];
		    ++nr;
		}
	    }
	}
	//
	// If the user drew profile from top to bottom, gotta reverse
	//
	else
	{
	    for (j = pcn-1; j >= 0; j--)
	    {
		profile[nr] = pc[j];
		++nr;
		// 
		// If the angle at an interior vertex is too big, we
		// create an extra vertex to get a sharp edge
		//
		if (j > 0 && j < pcn-1 &&
		    angleGreater(pc[j+1], pc[j], pc[j-1], cca))
		{
		    profile[nr] = pc[j];
		    ++nr;
		}
	    }
	}
	qmesh->verticesPerRow = nc;
	qmesh->verticesPerColumn = nr;
    }
    else	// Profile already figured out
    {
	nc = NumSides;
	qmesh->verticesPerRow.setValue(nc);
	nr = (int)qmesh->verticesPerColumn.getValue();
    }
	

    // Make sure we have enough room in coords/norms:
    coords->point.setNum(nr*nc);
    norms->vector.setNum(nr*nc);
    
    // Grab a pointer to efficiently edit coords/norms:
    SbVec3f *coordPtr = coords->point.startEditing();
    SbVec3f *normPtr = norms->vector.startEditing();

    //
    // Ok, figure out the first (and last) set of normals
    //
    for (j = 1; j < nr-1; j++)
    {
	SbVec3f n;
	figureNormal(profile[j-1], profile[j+1], n);
	normPtr[j*nc] = n;
	normPtr[j*nc + nc-1] = n;
    }
    //
    // And figure out the normals at the top and bottom
    //
    if (nr > 1)
    {
	SbVec3f n;
	figureNormal(profile[0], profile[1], n);
	normPtr[0] = n;
	normPtr[nc-1] = n;

	figureNormal(profile[nr-2], profile[nr-1], n);
	normPtr[nr*nc-nc] = n;
	normPtr[nr*nc-1] = n;
    }

    //
    // Ok, now that the initial profile and normals have been figured
    // out, just rotate them all about the y axis to get all of the
    // other points...
    //
    for (i = 0; i < nc; i++) {
	double angle = 2.0*M_PI * i / double(nc-1);
	double ca = cos(angle);
	double sa = sin(angle);

	// Set coordinates
	for (j = 0; j < nr; j++) {
	    coordPtr[j*nc + i] = SbVec3f(profile[j][0]*ca,
					 profile[j][1], profile[j][0]*sa);
	}
	// And normals
	if (i != 0 && i != nc-1) for (j = 0; j < nr; j++) {
	    normPtr[j*nc + i] =
		SbVec3f(norms->vector[j*nc][0]*ca,
			norms->vector[j*nc][1], norms->vector[j*nc][0]*sa);
	}
    }
    // Let Inventor know we're done modifying these for now:
    coords->point.finishEditing();
    norms->vector.finishEditing();
}

//
// Construct a new surface of revolution.  This sets up the scene
// graph (which will be returned by getSceneGraph())
//
RevolutionSurface::RevolutionSurface()
{
    root = new SoSeparator;
    root->ref();

    //
    // This combination of shape hints will turn on two-sided lighting
    // for us (if it is available)
    //
    SoShapeHints *hints = new SoShapeHints;
    root->addChild(hints);
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    hints->faceType = SoShapeHints::CONVEX;

    coords = new SoCoordinate3;
    root->addChild(coords);

    norms = new SoNormal;
    root->addChild(norms);

    SoNormalBinding *nb = new SoNormalBinding;
    root->addChild(nb);
    nb->value = SoNormalBinding::PER_VERTEX_INDEXED;

    qmesh = new SoQuadMesh;
    root->addChild(qmesh);

    NumSides = 20;	// Default...
}

//
// Destructor.  Must unref everything reffed in the constructor.
//
RevolutionSurface::~RevolutionSurface()
{
    root->unref();
}

void
RevolutionSurface::changeNumSides(int n)
{
    NumSides = n;

    createSurface(NULL);	// Re-calculate from old profile
}

SoSeparator *
RevolutionSurface::getSceneGraph()
{
    return root;
}
