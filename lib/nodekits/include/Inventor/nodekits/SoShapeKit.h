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
 |   $Revision: 1.1 $
 |
 |   Description:
 |      Defines the SoShapeKit class. 
 |      Subclassed off of SoSeparatorKit, this adds all the nodes potentially
 |      used to describe a shape. Not all of these nodes are needed for any
 |      one type of shape. For example, if you set the 'shape' part to be
 |      an SoSphere node, it will not make much sense to create the part
 |      called 'profileCoordinate3' since it will be ignored in drawing the
 |      sphere.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_SHAPE_KIT_
#define  _SO_SHAPE_KIT_

#include <Inventor/nodekits/SoSeparatorKit.h>

////////////////////////////////////////////////////////////////////
//    Class: SoShapeKit
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoShapeKit:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoShapeKit.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//    New nodes in this subclass are:
//         materialBinding, normalBinding,                 
//         and textureCoordinateBinding
//
//      A parent node that manages a collection of child nodes 
//      into a unit with the following structure:
//
//                            this
//              ----------------|
//              |        "topSeparator"
//         "callbackList"       |
//      ---------------------------------------------------------------
//      |       |           |     |         |    |   |               |
//  "pickStyle" |    "transform"  |         |    |   |               | 
//     "appearance"  "texture2Transform"    |    |   |               |
//                                          |    |  "localTransform" |
//                                          |    |                   |
//                                          |    |            "shapeSeparator"
//                                          |    |                   |
//                                          |    |                "shape"
//                                          |  "childList"
//          ---------------------------------                   
//          |        
// "materialBinding" "normalBinding"  
//  "textureCoordinateBinding" "shapeHints"
//  "coordinate3" "coordinate4" "normal"
//  "textureCoordinate2" "textureCoordinateFunction"
// "profileCoordinate2" "profileCoordinate3"
// "profileList" 
//

////////////////////////////////////////////////////////////////////
class SoShapeKit : public SoSeparatorKit {

    SO_KIT_HEADER(SoShapeKit);

    // defines fields for the new parts in the catalog
    // Binding nodes
    SO_KIT_CATALOG_ENTRY_HEADER(materialBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(normalBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(textureCoordinateBinding);

    // Shapehints
    SO_KIT_CATALOG_ENTRY_HEADER(shapeHints);

    // Coordinate and normal nodes
    SO_KIT_CATALOG_ENTRY_HEADER(coordinate3);
    SO_KIT_CATALOG_ENTRY_HEADER(coordinate4);
    SO_KIT_CATALOG_ENTRY_HEADER(normal);
    SO_KIT_CATALOG_ENTRY_HEADER(textureCoordinate2);
    SO_KIT_CATALOG_ENTRY_HEADER(textureCoordinateFunction);

    // Profile Stuff (for text and for nurbs)
    SO_KIT_CATALOG_ENTRY_HEADER(profileCoordinate2);
    SO_KIT_CATALOG_ENTRY_HEADER(profileCoordinate3);
    SO_KIT_CATALOG_ENTRY_HEADER(profileList);

    // Shape
    SO_KIT_CATALOG_ENTRY_HEADER(localTransform);
    SO_KIT_CATALOG_ENTRY_HEADER(shapeSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(shape);

  public:
    // constructor
    SoShapeKit();

  SoINTERNAL public:
    static void initClass();

  protected:

    // Called by the SoBaseKit::write() method. Calls setDefault(TRUE)
    // on the shapeSeparator. Note that this may be overriden later by basekit
    // if, for example, topSeparator lies on a path that is being written out.
    virtual void setDefaultOnNonWritingFields();

    virtual ~SoShapeKit();
};
#endif  /* _SO_SHAPE_KIT_ */
