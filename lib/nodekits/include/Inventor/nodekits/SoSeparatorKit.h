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
 |      Defines the SoSeparatorKit class. 
 |      Organizes an appearance, a transform, a local transform and a 
 |      group of child kits
 |
 |   Author(s)          : Paul Isaacs, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_SEPARATOR_KIT_
#define  _SO_SEPARATOR_KIT_

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/sensors/SoFieldSensor.h>

////////////////////////////////////////////////////////////////////
//    Class: SoSeparatorKit
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoSeparatorKit:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoSeparatorKit.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//    New nodes in this subclass are:
//         callbackList, topSeparator, pickStyle, appearance,                 
//          transform, texture2Transform, 
//          childList
//
//      A parent node that manages a collection of child nodes 
//      into a unit with the following structure:
//
//                            this
//            ------------------|
//            |          "topSeparator"
//         "callbackList"       |
//      ---------------------------------------------------------------
//      |       |       |           |     |                           |
//  "pickStyle" |    "units" "transform"  |                           |
//     "appearance"  "texture2Transform"                              |
//                                                                    |
//                                                                    | 
//                                                              "childList"
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSepKit
// C-api: public=renderCaching,boundingBoxCaching,renderCulling,pickCulling
class SoSeparatorKit : public SoBaseKit {

    SO_KIT_HEADER(SoSeparatorKit);

    // defines fields for the new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(appearance);
    SO_KIT_CATALOG_ENTRY_HEADER(units);
    SO_KIT_CATALOG_ENTRY_HEADER(transform);
    SO_KIT_CATALOG_ENTRY_HEADER(texture2Transform);
    SO_KIT_CATALOG_ENTRY_HEADER(childList);

  public:

    enum CacheEnabled {         // Possible values for caching
	OFF,                    // Never build or use a cache
	ON,                     // Always try to build a cache
	AUTO                    // Decide based on some heuristic
    };

    // Fields
    SoSFEnum renderCaching;     // OFF/ON/AUTO (see above)
    SoSFEnum boundingBoxCaching;// OFF/ON/AUTO (see above)
    SoSFEnum renderCulling;     // OFF/ON/AUTO (see above)
    SoSFEnum pickCulling;       // OFF/ON/AUTO (see above)

    // constructor
    SoSeparatorKit();

    // Overrides default method on SoNode
    virtual SbBool affectsState() const;

  SoINTERNAL public:
    static void initClass();

  protected:

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    // Called by the SoBaseKit::write() method. Calls setDefault(TRUE)
    // on the topSeparator. Note that this may be overriden later by basekit
    // if, for example, topSeparator lies on a path that is being written out.
    virtual void setDefaultOnNonWritingFields();

    // This sensor will watch the topSeparator part.  If the part changes to a 
    // new node,  then the fields of the old part will be disconnected and
    // the fields of the new part will be connected.
    // Connections are made from/to the renderCaching, boundingBoxCaching,
    // renderCulling and pickCulling fields. This way, the SoSeparatorKit
    // can be treated from the outside just like a regular SoSeparator node.
    // Setting the fields will affect caching and culling, even though the
    // topSeparator takes care of it.
    // oldTopSep keeps track of the part for comparison.
    SoFieldSensor *fieldSensor;
    static void fieldSensorCB( void *, SoSensor *);
    SoSeparator *oldTopSep;

    void connectSeparatorFields( SoSeparator *dest, SbBool onOff );

  protected:
    virtual ~SoSeparatorKit();

};
#endif  /* _SO_SEPARATOR_KIT_ */
