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
 |	SoBoxHighlightRenderAction
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <limits.h>
#include <math.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterialBinding.h>

#include <Inventor/actions/SoBoxHighlightRenderAction.h>

#include <Inventor/actions/SoWriteAction.h>

SO_ACTION_SOURCE(SoBoxHighlightRenderAction);

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor which takes no params
//
SoBoxHighlightRenderAction::SoBoxHighlightRenderAction()
	: SoGLRenderAction(SbVec2s(1, 1)) // pass a dummy viewport region
//
//////////////////////////////////////////////////////////////////////////////
{
    constructorCommon();
}

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor which takes SoGLRenderAction params
//
SoBoxHighlightRenderAction::SoBoxHighlightRenderAction(
    const SbViewportRegion &viewportRegion)
	: SoGLRenderAction(viewportRegion)
//
//////////////////////////////////////////////////////////////////////////////
{
    constructorCommon();
}

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor common
//
void
SoBoxHighlightRenderAction::constructorCommon()
//
//////////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoBoxHighlightRenderAction);

    // Set up our highlight graph
    localRoot   = new SoSeparator;
    lightModel	= new SoLightModel;
    baseColor	= new SoBaseColor;
    drawStyle	= new SoDrawStyle;
    texture	= new SoTexture2;
    xform	= new SoMatrixTransform;
    xlate	= new SoTranslation;
    cube	= new SoCube;
    SoMaterialBinding *mb = new SoMaterialBinding;
    
    SoComplexity *complexity = new SoComplexity;    
    complexity->value = 0.0;
    complexity->setOverride(TRUE);

    localRoot->ref();
    
    lightModel->model = SoLightModel::BASE_COLOR;
    lightModel->setOverride(TRUE);

    baseColor->rgb.setValue(1, 0, 0); // default color
    baseColor->setOverride(TRUE);

    drawStyle->style = SoDrawStyle::LINES;
    drawStyle->lineWidth = 3;
    drawStyle->linePattern = 0xffff;
    drawStyle->setOverride(TRUE);
    
    // turn off texturing
    texture->setOverride(TRUE);
    
    // set material binding to OVERALL:
    mb->setOverride(TRUE);
    
    // now set up the highlight graph
    localRoot->addChild(lightModel);
    localRoot->addChild(baseColor);
    localRoot->addChild(drawStyle);
    localRoot->addChild(texture);
    localRoot->addChild(complexity);
    localRoot->addChild(xform);
    localRoot->addChild(xlate);
    localRoot->addChild(cube);
    localRoot->addChild(mb);
    
    hlVisible = TRUE;
    selPath = NULL;
}    

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
SoBoxHighlightRenderAction::~SoBoxHighlightRenderAction()
//
//////////////////////////////////////////////////////////////////////////////
{
    localRoot->unref();
    if (selPath != NULL)
	selPath->unref();
}    

////////////////////////////////////////////////////////////////////////
//
// Update the matrix transform so that our cube fits the bbox of the
// passed path.
//
// Use: protected
//
void
SoBoxHighlightRenderAction::updateBbox(SoPath *p)
//
////////////////////////////////////////////////////////////////////////
{
    static SoGetBoundingBoxAction *bba = NULL;
    if (bba == NULL)
        bba = new SoGetBoundingBoxAction(getViewportRegion());    
    else
        bba->setViewportRegion( getViewportRegion());    
    bba->apply(p);    
    SbXfBox3f &box = bba->getXfBoundingBox();
    
    // Scale the cube to the correct size
    if (box.isEmpty()) {
	cube->width  = 0;
	cube->height = 0;
	cube->depth  = 0;
    }
    else {
	float x, y, z;
	box.getSize(x, y, z);
	cube->width  = x;
	cube->height = y;
	cube->depth  = z;
    }
    
    // Setup the matrix transform
    xform->matrix = box.getTransform();

    // If the bounding box is not centered at the origin, we have to
    // move the cube to the correct place. (To make the check, treat
    // the box as an SbBox3f, since we don't want the transformed
    // center.)
    const SbVec3f &min = ((SbBox3f &) box).getMin();
    const SbVec3f &max = ((SbBox3f &) box).getMax();
    if (min[0] != -max[0] || min[1] != -max[1] || min[2] != -max[2]) {
	xlate->translation.setValue(
	    (min[0] + max[0]) * .5, 
	    (min[1] + max[1]) * .5, 
	    (min[2] + max[2]) * .5);
	xlate->translation.setIgnored(FALSE);
    }
    else {
	// Translation field is not needed - data is centered about 0,0,0
	// and so is the cube.
	xlate->translation.setIgnored(TRUE);
    }
    
}

//////////////////////////////////////////////////////////////////////////////
//
//  beginTraversal - have the base class render the passed scene graph,
//  then render highlights for our selection node.
//
void
SoBoxHighlightRenderAction::apply(SoNode *renderRoot)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Render the scene
    SoGLRenderAction::apply(renderRoot);
    
    // Render the highlight?
    if (! hlVisible) return;
    
    // Is our cached path still valid?
    if ((selPath == NULL) ||
	(selPath->getHead() != renderRoot) ||
	(! selPath->getTail()->isOfType(SoSelection::getClassTypeId()))) {

	// Find the selection node under the render root
	static SoSearchAction *sa1 = NULL;
	if (sa1 == NULL)
	    sa1 = new SoSearchAction;
	else
	    sa1->reset();
	sa1->setFind(SoSearchAction::TYPE);
	sa1->setInterest(SoSearchAction::FIRST);
	sa1->setType(SoSelection::getClassTypeId());
	sa1->apply(renderRoot);
	
	// Cache this path
	if (selPath != NULL)
	    selPath->unref();
	selPath = sa1->getPath();
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
	    SoFullPath *p = (SoFullPath *) sel->getPath(j);	    
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
    
	    // Find the camera used to render the selected object and
	    // insert it into the highlight graph as the first child
	    SoNode *camera;

	    static SoSearchAction *sa2 = NULL;
	    if (sa2 == NULL)
		sa2 = new SoSearchAction;
	    else
		sa2->reset();

	    sa2->setFind(SoSearchAction::TYPE);
	    sa2->setInterest(SoSearchAction::LAST);
	    sa2->setType(SoCamera::getClassTypeId());
	    sa2->apply(selPath);
	    camera =(sa2->getPath() == NULL ? NULL : sa2->getPath()->getTail());
	    if (camera != NULL)
		localRoot->insertChild(camera, 0);
	    
	    // Get the bounding box of the object and update the
	    // local highlight graph
	    updateBbox(selPath);
	    
	    // Make sure the box has some size
	    if ((cube->width.getValue() == 0) &&
		(cube->height.getValue() == 0) &&
		(cube->depth.getValue() == 0)) {
#ifdef DEBUG
		SoDebugError::postWarning("SoBoxHighlightRenderAction::apply",
			"selected object has no bounding box - no highlight rendered");
#endif		    
	    }
	    else {
		// Render the highlight
		SoGLRenderAction::apply(localRoot);
	    }
	    	    
	    // Restore selPath for reuse
	    selPath->truncate(reusablePathLength);

	    // Remove the camera for the next path
	    if (camera != NULL)
		localRoot->removeChild(0);
	}
    }
}    

// Methods which affect highlight appearance
void
SoBoxHighlightRenderAction::setColor( const SbColor &c )
{ baseColor->rgb.setValue(c); }

const SbColor &
SoBoxHighlightRenderAction::getColor()
{ return baseColor->rgb[0]; }

void
SoBoxHighlightRenderAction::setLinePattern( unsigned short pattern )
{ drawStyle->linePattern = pattern; }

unsigned short
SoBoxHighlightRenderAction::getLinePattern()
{ return drawStyle->linePattern.getValue(); }

void
SoBoxHighlightRenderAction::setLineWidth( float width )
{ drawStyle->lineWidth = width; }

float
SoBoxHighlightRenderAction::getLineWidth()
{ return drawStyle->lineWidth.getValue(); }

//
// These are here to quiet the compiler.
//
void
SoBoxHighlightRenderAction::apply(SoPath *path)
{ SoGLRenderAction::apply(path); }

void
SoBoxHighlightRenderAction::apply(const SoPathList &pathList, SbBool obeysRules)
{ SoGLRenderAction::apply(pathList, obeysRules); }
