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

//  -*- C++ -*-

/*
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This is the SoBoxHighlightRenderAction class, used to highlight
 |   the selection by drawing a wireframe box around the selected shapes.
 |
 |   Classes	: SoBoxHighlightRenderAction
 |
 |   Author(s)	: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#ifndef _SO_BOX_HIGHLIGHT_RENDER_ACTION_
#define _SO_BOX_HIGHLIGHT_RENDER_ACTION_

#include <Inventor/actions/SoGLRenderAction.h>

class SbColor;
class SoSeparator;
class SoLightModel;
class SoBaseColor;
class SoDrawStyle;
class SoTexture2;
class SoTranslation;
class SoMatrixTransform;
class SoCube;

//////////////////////////////////////////////////////////////////////////////
//
// Box highlight - a subclass of SoGLRenderAction which renders the
// scene graph, then renders wireframe boxes surrounding each selected object.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoBoxHLRenderAct
class SoBoxHighlightRenderAction : public SoGLRenderAction {
      SO_ACTION_HEADER(SoBoxHighlightRenderAction);
 public:
    // Constructor which takes no parameters. This sets up a dummy
    // viewport region. If passed to a render area, the render area
    // will correctly set the region. Otherwise, setViewportRegion()
    // should be called before applying this action.
    SoBoxHighlightRenderAction();
    
    // Constructor which takes the normal SoGLRenderAction parameters.
    // C-api: name=CreateVpRgn
    SoBoxHighlightRenderAction(const SbViewportRegion &viewportRegion);
		  
    
    virtual ~SoBoxHighlightRenderAction();
    
    // Applies action to the graph rooted by a node. 
    // This renders the scene, then renders highlights for selected objects.
    virtual void    apply(SoNode *node);

    // Applies action to the graph defined by a path or path list.
    // These simply invoke the parent class apply() methods.
    // These do NOT highlight the path, whether selected or not.
    virtual void    apply(SoPath *path);
    virtual void    apply(const SoPathList &pathList,
			      SbBool obeysRules = FALSE);
    
    // Simple switch to turn highlighting on (TRUE) or off (FALSE)
    // App is responsible for redrawing after this state changes.
    void	    setVisible(SbBool b) { hlVisible = b; }
    SbBool	    isVisible() const { return hlVisible; }

    // Set the appearance of the highlight.
    // Application is responsible for redrawing the scene
    // after making changes here.
    // C-api: name=setCol
    void	    setColor( const SbColor &c );
    // C-api: name=getCol
    const SbColor & getColor();
    // C-api: name=setLinePat
    void            setLinePattern( unsigned short pattern );
    // C-api: name=getLinePat
    unsigned short  getLinePattern();
    void            setLineWidth( float width );
    float           getLineWidth();
   
 SoINTERNAL public: 
    static void initClass();

  protected:
    void updateBbox(SoPath *p);
    
    // Nodes which comprise the local highlight graph
    SoSeparator	    *localRoot;
    SoLightModel    *lightModel;
    SoBaseColor	    *baseColor;
    SoDrawStyle	    *drawStyle;
    SoTexture2	    *texture;
    SoTranslation   *xlate;
    SoMatrixTransform *xform;
    SoCube	    *cube;
    
    SbBool	    hlVisible;
    
    // Cached path to selection so we don't search every time
    SoPath	    *selPath;
    
  private:
    void	    constructorCommon();
};

#endif /* _SO_BOX_HIGHLIGHT_RENDER_ACTION_ */

