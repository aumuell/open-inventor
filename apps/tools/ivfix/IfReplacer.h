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

/////////////////////////////////////////////////////////////////////////////
//
// IfReplacer class: replaces nodes we don't want to deal with with more
// friendly versions.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_REPLACER_
#define  _IF_REPLACER_

#include <Inventor/actions/SoCallbackAction.h>

class SoMaterial;
class SoNode;
class SoPath;

class IfReplacer {

  public:
    IfReplacer();
    ~IfReplacer();

    // Replaces nodes in the given scene, in place
    void	replace(SoNode *sceneRoot);

  private:
    SoMaterial	*material;		// Replacement material

    // Replaces all nodes of the given type with SoMaterial nodes
    void	replaceMaterials(SoNode *sceneRoot,
				 const SoType &typeToReplace);

    // Creates and returns a material node that represents the
    // material in effect at the tail of the given path.
    SoMaterial * createMaterialForPath(SoPath *path);

    // Stores a material created during traversal of a path
    void	storeMaterial(SoCallbackAction *cba);

    // Callback
    static SoCallbackAction::Response materialTailCB(void *userData,
						     SoCallbackAction *cba,
						     const SoNode *)
	{ ((IfReplacer *) userData)->storeMaterial(cba);
	  return SoCallbackAction::CONTINUE; }
};

#endif /* _IF_REPLACER_ */
