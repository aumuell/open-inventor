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
 *  chapter 4, example 1.
 *
 *  Header file for "GetVolumeAction" action.
 *------------------------------------------------------------*/

#include <Inventor/actions/SoSubAction.h>

class GetVolumeAction : public SoAction {

   SO_ACTION_HEADER(GetVolumeAction);

 public:
   // Initializes this action class for use with scene graphs
   static void    initClass();

   // Constructor and destructor
   GetVolumeAction();
   virtual ~GetVolumeAction();

   // Returns computed volume after action is applied
   float          getVolume() const { return volume; }

 protected:
   // Initiates action on graph
   virtual void   beginTraversal(SoNode *node);

 private:
   float          volume;      // Computed volume

   // These are the methods that are used to apply the action
   // to various node classes. The third method is registered
   // for all relevant non-shape nodes. The calling sequence for
   // these methods is that used for all methods in the global
   // action table.
   static void    cubeVolume(SoAction *, SoNode *);
   static void    sphereVolume(SoAction *, SoNode *);
   static void    callDoAction(SoAction *, SoNode *);

   // This adds the given object-space volume to the total. It
   // first converts the volume to world space, using the
   // current model matrix.
   void           addVolume(float objectSpaceArea);
};
