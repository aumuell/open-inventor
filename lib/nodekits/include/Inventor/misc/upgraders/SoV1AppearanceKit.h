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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |      Defines the SoV1AppearanceKit class. A parent node that manages 
 |      a collection of child nodes for
 |      complete description of the graphical appearance.
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_V1_APPEARANCE_KIT_
#define  _SO_V1_APPEARANCE_KIT_

#include <Inventor/misc/upgraders/SoV1BaseKit.h>
#include <Inventor/SoLists.h>


////////////////////////////////////////////////////////////////////
//    Class: SoV1AppearanceKit 
//
//      A parent node that manages a collection of child nodes
//      for complete description of the graphical appearance.
//
//    New nodes in this subclass are:
//         lightModel, drawStyle, material, complexity, texture2, and font
//
//    The structure of the catalog for this class is:
//
//                       this
//                         |
//   -----------------------------------------------------------------------
//   |         |           |      |        |        |             |        |
//  "label"    |    "environment" |"material" "complexity"        |     "font"
//             |                  |                    "texture2list"
//        "lightModel"     "drawStyle"        
//
//
////////////////////////////////////////////////////////////////////

SoEXTENDER class SoV1AppearanceKit : public SoV1BaseKit {

    // Define typeId and name stuff
    SO_NODE_HEADER(SoV1AppearanceKit);

    // Define catalog for children
    SO_V1_SUBKIT_CATALOG_HEADER(SoV1AppearanceKit);

  public:
    // constructor
    SoV1AppearanceKit();

    virtual SoNode *createNewNode();

    // If tryToSetPartInNewNode fails, then this routine is called.
    // It will fail if the part read from file was called "texture2List"
    // This part has been changed to a single noded part, "texture2"
    // We will use just the first child of the list and set it as the "texture2"
    // It will also print a warning.
    virtual SbBool dealWithUpgradedPart( SoBaseKit *newNode,
				  SoNode *newPart, const SbName &newPartName );
  SoINTERNAL public:
    static void initClass();

    static SoNodeList *getKitsWithUnusedTextureXfs();
    static SoNodeList *getUnusedTextureXfs();

  protected:

    // In the case where an SoV1Texture2 node has the translation rotation
    // scaleFactor or center field set, it will return a group containing
    // an SoTexture2Transform and an SoTexture2 node.
    // The SoV1AppearanceKit can not have a group part, so it pulls out the
    // SoTexture2 node and sets it as "transform."
    // Instead of just throwing away the SoTexture2Transform, it puts it in
    // this static variable so that other nodes can get at it.
    // For example, the SoV1GroupKit overloads the virtual function 
    // setUpNewNode to get this texture2Transform and copy its
    // values into its "texture2Transform" part.
    static SoNodeList *kitsWithUnusedTextureXfs;
    static SoNodeList *unusedTextureXfs;

    virtual ~SoV1AppearanceKit();
};

#endif  /* _SO_V1_APPEARANCE_KIT_ */
