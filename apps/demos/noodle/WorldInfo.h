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

/*
 * Copyright (C) 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
/*
 |   Description:
 |	This file defines the WorldInfo class. This is a separator
 |      with a selection node and a set of noodle under it.
 |      The WorldInfo class takes care of managing the noodles it contains.
 |
 |   Author(s): Paul Isaacs
 |
 */

#ifndef  _SO_WORLD_INFO_
#define  _SO_WORLD_INFO_

#include <Inventor/SbLinear.h>
#include <Inventor/SoLists.h>
#include <Inventor/nodes/SoSelection.h>

class GeneralizedCylinder;
class SoSeparator;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: WorldInfo
//
//////////////////////////////////////////////////////////////////////////////

class WorldInfo {
    
  public:

    WorldInfo();
    ~WorldInfo();

    SoSeparator *getWorldRoot() { return worldRoot; }

    // An argument of NULL will create an empty separator for the scene.
    void setScene( SoSeparator *newScene );
    SoSeparator *getScene() { return sceneRoot; }
    SbBool isSceneEmpty();

    char *getFileName() { return fileName; }
    void    setFileName( char *newFileName );



    // Noodles may be added or deleted.
    void addNoodle( GeneralizedCylinder *newNoodle );
    void deleteNoodle( GeneralizedCylinder *noodleToRemove );

    void deleteCurrentNoodle();
    // Returns tghe new selection.
    GeneralizedCylinder * undeleteNoodle();

    GeneralizedCylinder *setFirstNoodleCurrent();
    GeneralizedCylinder *addNewNoodle();

    void setCurrentNoodle( GeneralizedCylinder *newNoodle );
    GeneralizedCylinder *getCurrentNoodle() { return currentNoodle; }

    SoType  getManipType() { return manipType; }
    void    setManipType( SoType newType );
 
    SoSeparator *getVanillaSceneCopy();

    SoSelection *getSelectorNode() { return selector; }

  protected:

    SoSeparator  *worldRoot;
    SoSelection  *selector;
    SoSeparator  *sceneRoot;

    static void selectionCB( void *, SoPath *);
    static void deselectionCB( void *, SoPath *);
    static SoPath *pickFilterCB( void *, const SoPickedPoint *);

  private:
    GeneralizedCylinder *currentNoodle;
    char                *fileName;
    SoType              manipType;

    // Stores deleted pieces, most recently deleted added to end.
    // When undelete() is called, last entry is removed and put into scene.
    SoNodeList *deletedNoodles;
};

#endif /* _SO_WORLD_INFO_ */
