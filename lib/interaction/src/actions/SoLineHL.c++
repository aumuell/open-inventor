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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoLineHighlightRenderAction
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoPath.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterialBinding.h>

#include <Inventor/actions/SoLineHighlightRenderAction.h>


SO_ACTION_SOURCE(SoLineHighlightRenderAction);

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor which takes no params
//
SoLineHighlightRenderAction::SoLineHighlightRenderAction()
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
SoLineHighlightRenderAction::SoLineHighlightRenderAction(
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
SoLineHighlightRenderAction::constructorCommon()
//
//////////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoLineHighlightRenderAction);

    // Set up our highlight graph
    localRoot   = new SoSeparator;
    lightModel	= new SoLightModel;
    baseColor	= new SoBaseColor;
    drawStyle	= new SoDrawStyle;
    texture	= new SoTexture2;
    SoMaterialBinding *mb = new SoMaterialBinding;

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
    
    //set material binding to OVERALL
    mb->setOverride(TRUE);
    
    // now set up the highlight graph
    localRoot->addChild(lightModel);
    localRoot->addChild(baseColor);
    localRoot->addChild(drawStyle);
    localRoot->addChild(texture);
    localRoot->addChild(mb);
    
    hlVisible = TRUE;
}    

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
SoLineHighlightRenderAction::~SoLineHighlightRenderAction()
//
//////////////////////////////////////////////////////////////////////////////
{
    localRoot->unref();
}    

//////////////////////////////////////////////////////////////////////////////
//
//  beginTraversal - have the base class render the passed scene graph,
//  then render highlights for our selection node.
//
void
SoLineHighlightRenderAction::apply(SoNode *node)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Render the scene
    SoGLRenderAction::apply(node);

    // Render the highlight?
    if (! hlVisible) return;

    // Add the rendering localRoot beneath our local scene graph localRoot
    // so that we can find a path from localRoot to the selection node 
    // which is under the render root.
    localRoot->addChild(node);
    
    // Find the selection node under the local root
    static SoSearchAction *sa = NULL;
    if (sa == NULL)
	sa = new SoSearchAction;
    else
	sa->reset();
    sa->setFind(SoSearchAction::TYPE);
    sa->setInterest(SoSearchAction::FIRST);
    sa->setType(SoSelection::getClassTypeId());
    sa->apply(localRoot);
    
    SoPath *hlPath = sa->getPath();
    if (hlPath != NULL) {
	hlPath = hlPath->copy();
	hlPath->ref();
	
	// Make sure something is selected
	SoSelection *sel = (SoSelection *) hlPath->getTail();
	if (sel->getNumSelected() > 0) {
	    // Keep the length from the root to the selection
	    // as an optimization so we can reuse this data
	    int reusablePathLength = hlPath->getLength();
    
	    // For each selection path, create a new path rooted under our localRoot
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
			hlPath->append(fp->getIndex(++k));
		    } 
		    while ( fp->getNode(k) != kitTail );
		}
		else {
		    for (int k = 1; k < p->getLength(); k++)
			hlPath->append(p->getIndex(k));
		}
	
		// Render the shape with the local draw style to make the highlight
		SoGLRenderAction::apply(hlPath);
			
		// Restore hlPath for reuse
		hlPath->truncate(reusablePathLength);
	    }
	}
	
	hlPath->unref();
    }
    
    // Remove the rendering localRoot from our local scene graph
    localRoot->removeChild(node);
}    


// Methods which affect highlight appearance
void
SoLineHighlightRenderAction::setColor( const SbColor &c )
{ baseColor->rgb.setValue(c); }

const SbColor &
SoLineHighlightRenderAction::getColor()
{ return baseColor->rgb[0]; }

void
SoLineHighlightRenderAction::setLinePattern( unsigned short pattern )
{ drawStyle->linePattern = pattern; }

unsigned short
SoLineHighlightRenderAction::getLinePattern()
{ return drawStyle->linePattern.getValue(); }

void
SoLineHighlightRenderAction::setLineWidth( float width )
{ drawStyle->lineWidth = width; }

float
SoLineHighlightRenderAction::getLineWidth()
{ return drawStyle->lineWidth.getValue(); }


//
// These are here to quiet the compiler.
//
void
SoLineHighlightRenderAction::apply(SoPath *path)
{ SoGLRenderAction::apply(path); }

void
SoLineHighlightRenderAction::apply(const SoPathList &pathList, SbBool obeysRules)
{ SoGLRenderAction::apply(pathList, obeysRules); }
