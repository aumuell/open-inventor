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
 |      Defines the SoAppearanceKit class. A parent node that manages 
 |      a collection of child nodes for
 |      complete description of the graphical appearance.
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_APPEARANCE_KIT_
#define  _SO_APPEARANCE_KIT_

#include <Inventor/nodekits/SoBaseKit.h>


////////////////////////////////////////////////////////////////////
//    Class: SoAppearanceKit 
//
//      A parent node that manages a collection of child nodes
//      for complete description of the graphical appearance.
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoAppearanceKit:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoAppearanceKit.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//    The structure of the catalog for this class is:
//
//                       this
//                         |
//   -----------------------------------------------------------------------
//   |            |             |      |        |        |             |
//  "callbackList"|"environment"|"material" "complexity" |           "font"
//                |             |                      "texture2" 
//        "lightModel"     "drawStyle"                            
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoAppearKit
class SoAppearanceKit : public SoBaseKit {

    SO_KIT_HEADER(SoAppearanceKit);

    // defines fields for the new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
    SO_KIT_CATALOG_ENTRY_HEADER(environment);
    SO_KIT_CATALOG_ENTRY_HEADER(drawStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(material);
    SO_KIT_CATALOG_ENTRY_HEADER(complexity);
    SO_KIT_CATALOG_ENTRY_HEADER(texture2);
    SO_KIT_CATALOG_ENTRY_HEADER(font);

  public:
    // constructor
    SoAppearanceKit();

  SoINTERNAL public:
    static void initClass();

  protected:
    virtual ~SoAppearanceKit();
};

#endif  /* _SO_APPEARANCE_KIT_ */
