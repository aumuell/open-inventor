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
 *  Header file for highlight that appears in the overlay planes.
 *------------------------------------------------------------*/

#ifndef _OVERLAY_HIGHLIGHT_RENDER_ACTION_
#define _OVERLAY_HIGHLIGHT_RENDER_ACTION_

#include <Inventor/actions/SoGLRenderAction.h>

class SoCamera;
class SoGLRenderAction;
class SoPath;
class SoSeparator;
class SoOrthographicCamera;
class SoCoordinate3;

class OverlayHighlightRenderAction : public SoGLRenderAction {
      SO_ACTION_HEADER(OverlayHighlightRenderAction);
  public:
   // Constructor and destructor
   OverlayHighlightRenderAction();
   ~OverlayHighlightRenderAction();
   
   // Applies action to the graph rooted by a node,
   // only drawing selected objects.
   virtual void    apply(SoNode *node);

   // Applies action to the graph defined by a path or path list.
   // These simply invoke the parent class apply() methods.
   // These do NOT highlight the path, whether selected or not.
   // They are implemented to keep the compiler happy.
   virtual void    apply(SoPath *path);
   virtual void    apply(const SoPathList &pathList,
			 SbBool obeysRules = FALSE);

   // This must be called before this class is used.
   static void initClass();
   
  protected:
   void		    updateBbox(SoPath *p, SoCamera *c);
    
   // Local scene graph
   SoSeparator		*localRoot;
   SoOrthographicCamera	*orthoCam;
   SoCoordinate3	*coords;
    
   // We will cache the path to the first selection node
   SoPath		*selPath;
};

#endif /* _OVERLAY_HIGHLIGHT_RENDER_ACTION_ */
