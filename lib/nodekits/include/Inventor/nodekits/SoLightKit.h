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
 |      Defines the SoLightKit class. 
 |      This is the base class for all other lights.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_LIGHT_KIT_
#define  _SO_LIGHT_KIT_

#include <Inventor/nodekits/SoBaseKit.h>

////////////////////////////////////////////////////////////////////
//    Class: SoLightKit
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoLightKit:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoLightKit.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//      A parent node that manages a collection of child nodes 
//      into a unit with the following structure:
//
//                            this
//                              |
//     --------------------------
//     |                        |         
//     |                    "transformGroup"
//"callbackList"                |
//               ---------------------------------
//               |               |               |
//              "transform"  "light"       "iconSeparator"
//                                               |
//                                            "icon"
//                                           
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoLtKit
class SoLightKit : public SoBaseKit {

    SO_KIT_HEADER(SoLightKit);

    // defines fields for the new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(transformGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(transform);
    SO_KIT_CATALOG_ENTRY_HEADER(light);
    SO_KIT_CATALOG_ENTRY_HEADER(iconSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(icon);

  public:
    // constructor
    SoLightKit();

  SoINTERNAL public:
    static void initClass();

  protected:
    virtual ~SoLightKit();
};
#endif  /* _SO_LIGHT_KIT_ */
