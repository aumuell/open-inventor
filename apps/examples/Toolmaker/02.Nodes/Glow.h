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
 *  chapter 2, example 1.
 *
 *  Header file for "Glow" property node.
 *------------------------------------------------------------*/

#include <Inventor/SbColor.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoSubNode.h>

// An SoColorPacker is needed to put the transparency into the state:
class SoColorPacker;

class Glow : public SoNode {

   SO_NODE_HEADER(Glow);

 public:
   // Fields:
   SoSFColor      color;         // Color of glow
   SoSFFloat      brightness;    // Amount of glow (0-1)
   SoSFFloat	  transparency;	 // transparency of glowing object 

   // Initializes this class for use in scene graphs. This
   // should be called after database initialization and before
   // any instance of this node is constructed.
   static void    initClass();

   // Constructor
   Glow();

 protected:
   // These implement supported actions. The only actions that
   // deal with materials are the callback and GL render
   // actions. We will inherit all other action methods from
   // SoNode.
   virtual void   GLRender(SoGLRenderAction *action);
   virtual void   callback(SoCallbackAction *action);

   // This implements generic traversal of Glow node, used in
   // both of the above methods.
   virtual void   doAction(SoAction *action);

 private:
   // Destructor. Private to keep people from trying to delete
   // nodes, rather than using the reference count mechanism.
   virtual ~Glow();

   // A pointer to the color packer is required in nodes that set diffuse
   // or transparency.  This will be initialized in constructor, deleted in
   // destructor:
   
   SoColorPacker	*colorPacker;
   
   // Keep a copy of the transparency that this node puts into the state:
   float		transpValue;
   
};
