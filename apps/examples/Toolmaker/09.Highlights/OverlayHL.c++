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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 9.
 *
 *  Source file for highlight that appears in the overlay planes.
 *------------------------------------------------------------*/


#include <limits.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>

#include "OverlayHighlightRenderAction.h"

SO_ACTION_SOURCE(OverlayHighlightRenderAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the OverlayHighlightRenderAction class.
//
void
OverlayHighlightRenderAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(OverlayHighlightRenderAction, SoGLRenderAction);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
OverlayHighlightRenderAction::OverlayHighlightRenderAction()
	: SoGLRenderAction(SbVec2s(1, 1)) // pass a dummy viewport region
//
//////////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(OverlayHighlightRenderAction);

    selPath = NULL;
    
    // Set up the local rendering graph
    localRoot = new SoSeparator;
    localRoot->ref();
    
    SoPickStyle *pickStyle = new SoPickStyle;
    pickStyle->style = SoPickStyle::UNPICKABLE;
    localRoot->addChild(pickStyle);
    
    // Set up camera to look at 0 <= x,y <= 1
    orthoCam = new SoOrthographicCamera;
    orthoCam->position.setValue(.5, .5, 1.);
    orthoCam->height = 1.0;
    localRoot->addChild(orthoCam);
    
    SoLightModel *lmodel = new SoLightModel;
    lmodel->model = SoLightModel::BASE_COLOR;
    localRoot->addChild(lmodel);

    SoColorIndex *color = new SoColorIndex;
    color->index = 1;
    localRoot->addChild(color);

    SoDrawStyle *drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyle::LINES;
    drawStyle->lineWidth = 3;
    drawStyle->linePattern = 0xffff;
    localRoot->addChild(drawStyle);
    
    coords = new SoCoordinate3;
    coords->point.setNum(0);
    localRoot->addChild(coords);
    
    SoFaceSet *fset = new SoFaceSet;
    fset->numVertices = 4;
    localRoot->addChild(fset);
}    

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
OverlayHighlightRenderAction::~OverlayHighlightRenderAction()
//
//////////////////////////////////////////////////////////////////////////////
{
    localRoot->unref();
    if (selPath != NULL)
	selPath->unref();
}    

////////////////////////////////////////////////////////////////////////
//
// Update the bbox to surround the projected bounding box of the path.
//
// Use: protected
//
void
OverlayHighlightRenderAction::updateBbox(SoPath *p, SoCamera *camera)
//
////////////////////////////////////////////////////////////////////////
{
    coords->point.deleteValues(0); // clear them all out
    
    if (camera == NULL) return;
    
    // Compute the 3d bounding box of the passed path
    SoGetBoundingBoxAction bba(getViewportRegion());    
    bba.apply(p); 
    SbVec3f min, max;
    bba.getBoundingBox().getBounds(min, max); 
    
    // Project points to (0 <= x,y,z <= 1) screen coordinates
    SbViewVolume vv = camera->getViewVolume();
    SbVec3f screenPoint[8];
    vv.projectToScreen(SbVec3f(min[0], min[1], min[2]), screenPoint[0]);
    vv.projectToScreen(SbVec3f(min[0], min[1], max[2]), screenPoint[1]);
    vv.projectToScreen(SbVec3f(min[0], max[1], min[2]), screenPoint[2]);
    vv.projectToScreen(SbVec3f(min[0], max[1], max[2]), screenPoint[3]);
    vv.projectToScreen(SbVec3f(max[0], min[1], min[2]), screenPoint[4]);
    vv.projectToScreen(SbVec3f(max[0], min[1], max[2]), screenPoint[5]);
    vv.projectToScreen(SbVec3f(max[0], max[1], min[2]), screenPoint[6]);
    vv.projectToScreen(SbVec3f(max[0], max[1], max[2]), screenPoint[7]);
    
    // Find the encompassing 2d box (0 <= x,y <= 1)
    SbBox2f bbox2;
    for (int i = 0; i < 8; i++)
	bbox2.extendBy(SbVec2f(screenPoint[i][0], screenPoint[i][1]));
    
    if (! bbox2.isEmpty()) {
	float xmin, ymin, xmax, ymax;
	bbox2.getBounds(xmin, ymin, xmax, ymax);
			      
	// Set up the coordinate node
	coords->point.set1Value(0,  xmin, ymin, 0);
	coords->point.set1Value(1,  xmax, ymin, 0);
	coords->point.set1Value(2,  xmax, ymax, 0);
	coords->point.set1Value(3,  xmin, ymax, 0);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  beginTraversal - render highlights for our selection node.
//
void
OverlayHighlightRenderAction::apply(SoNode *renderRoot)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Do not render the scene - only render the highlights
    
    // Is our cached path still valid?
    if ((selPath == NULL) ||
	(selPath->getHead() != renderRoot) ||
	(! selPath->getTail()->isOfType(SoSelection::getClassTypeId()))) {

	// Find the selection node under the render root
	SoSearchAction sa;
	sa.setFind(SoSearchAction::TYPE);
	sa.setInterest(SoSearchAction::FIRST);
	sa.setType(SoSelection::getClassTypeId());
	sa.apply(renderRoot);
	
	// Cache this path
	if (selPath != NULL)
	    selPath->unref();
	selPath = sa.getPath();
	if (selPath != NULL) {
	    selPath = selPath->copy();
	    selPath->ref();
	}
    }
    
    if (selPath != NULL) {
	// Make sure something is selected
	SoSelection *sel = (SoSelection *) selPath->getTail();
	if (sel->getNumSelected() == 0) return;
	
	// Keep the length from the root to the selection
	// as an optimization so we can reuse this data
	int reusablePathLength = selPath->getLength();

	// For each selection path, create a new path rooted under our
	// localRoot
	for (int j = 0; j < sel->getNumSelected(); j++) {
	    // Continue the path down to the selected object.
	    // No need to deal with p[0] since that is the sel node.
	    SoPath *p = sel->getPath(j);	    
	    SoNode *pathTail = p->getTail();

	    if ( pathTail->isOfType(SoBaseKit::getClassTypeId())) {
		// Find the last nodekit on the path.
		SoNode *kitTail = ((SoNodeKitPath *)p)->getTail();

		// Extend the selectionPath until it reaches this last kit.
		SoFullPath *fp = (SoFullPath *) p;
		int k = 0;
		do {
		    selPath->append(fp->getIndex(++k));
		} 
		while ( fp->getNode(k) != kitTail );
	    }
	    else {
		for (int k = 1; k < p->getLength(); k++)
		    selPath->append(p->getIndex(k));
	    }
    
	    // Find the camera used to render the selected object
	    SoNode *camera;
	    SoSearchAction sa;
	    sa.setFind(SoSearchAction::TYPE);
	    sa.setInterest(SoSearchAction::LAST);
	    sa.setType(SoCamera::getClassTypeId());
	    sa.apply(selPath);
	    camera = (sa.getPath() == NULL ? NULL : sa.getPath()->getTail());
	    
	    // Get the bounding box of the object and update the
	    // local highlight graph
	    updateBbox(selPath, (SoCamera *)camera);
	    
	    // Make sure the box has some size
	    if (coords->point.getNum() == 0) {
#ifdef DEBUG
		SoDebugError::post("OverlayHighlightRenderAction::apply",
			"selected object has no bounding box - cannot render a highlight");
#endif		    
	    }
	    else {
		// Render the highlight
		SoGLRenderAction::apply(localRoot);
	    }
	    	    
	    // Restore selPath for reuse
	    selPath->truncate(reusablePathLength);
	}
    }
}    

//
// Function stubs: we do not highlight paths and pathLists.
//
void
OverlayHighlightRenderAction::apply(SoPath *path)
{ SoGLRenderAction::apply(path); }

void
OverlayHighlightRenderAction::apply(const SoPathList &pathList, SbBool obeysRules)
{ SoGLRenderAction::apply(pathList, obeysRules); }
