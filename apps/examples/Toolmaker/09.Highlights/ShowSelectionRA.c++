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
 *  Source file for "ShowSelection" highlight.
 *------------------------------------------------------------*/


#include <Inventor/SoPath.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>

#include "ShowSelectionRA.h"


SO_ACTION_SOURCE(ShowSelectionRenderAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the ShowSelectionRenderAction class.
//
void
ShowSelectionRenderAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
   SO_ACTION_INIT_CLASS(ShowSelectionRenderAction, SoGLRenderAction);
}

////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
ShowSelectionRenderAction::ShowSelectionRenderAction()
	: SoGLRenderAction(SbVec2s(1, 1)) // pass a dummy viewport region
//
////////////////////////////////////////////////////////////////////////
{
   selPath = NULL;
}    

////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
ShowSelectionRenderAction::~ShowSelectionRenderAction()
//
////////////////////////////////////////////////////////////////////////
{
   if (selPath != NULL)
      selPath->unref();
}    

////////////////////////////////////////////////////////////////////////
//
//  Render the passed scene by searching for the first selection node,
//  then rendering only the selected objects.
//
void
ShowSelectionRenderAction::apply(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
   node->ref();
   
   // Do we have to search for the selection node?
   // Only if our cached path is NULL, 
   // or the action is being applied to a different scene,
   // or the tail of our existing path is no longer a selection
   // node (for instance if that node was removed from the scene).
   if ((selPath == NULL) ||
       (selPath->getHead() != node) ||
       (! selPath->getTail()->isOfType(
              SoSelection::getClassTypeId()))) {
   
      // Find the first selection node under the passed root
      SoSearchAction sa;
      sa.setFind(SoSearchAction::TYPE);
      sa.setInterest(SoSearchAction::FIRST);
      sa.setType(SoSelection::getClassTypeId());
      sa.apply(node);
   
      // Cache this new path
      if (selPath != NULL)
         selPath->unref();
      selPath = sa.getPath();
      if (selPath != NULL) {
         selPath = selPath->copy();
         selPath->ref();
      }
   }
   
   // Render the selected paths!
   if (selPath != NULL) {	
      SoSelection *sel = (SoSelection *) selPath->getTail();
      if (sel->getNumSelected() > 0) {
         // Keep the length from the root to the selection
         // as an optimization so we can reuse this data
         int reusablePathLength = selPath->getLength();

         // For each selection path, we need the full path from
	 // the passed root to render, else we may not have a camera.
         for (int j = 0; j < sel->getNumSelected(); j++) {
            // Continue the path down to the selected object.
            // No need to deal with p[0] since that is the sel node.
            SoPath *p = sel->getPath(j);
            for (int k = 1; k < p->getLength(); k++)
               selPath->append(p->getIndex(k));

            // Render the selected shape
            SoGLRenderAction::apply(selPath);
	
            // Restore selPath for reuse
            selPath->truncate(reusablePathLength);
         }
      }
   }
   
   node->unref();
}    

//
// Function stubs: we do not highlight paths and pathLists.
//
void
ShowSelectionRenderAction::apply(SoPath *path)
{ SoGLRenderAction::apply(path); }

void
ShowSelectionRenderAction::apply(const SoPathList &pathList, SbBool obeysRules)
{ SoGLRenderAction::apply(pathList, obeysRules); }
