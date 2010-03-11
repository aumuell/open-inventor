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
 |	Upgrade from 1.0 Texture2 to 2.0
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_V1TEXTURE2_
#define  _SO_V1TEXTURE2_

#include <Inventor/misc/upgraders/SoUpgrader.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFFloat.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoV1Texture2
//
//  V1Texture2 for upgrading 1.0 Texture2s to the 2.0 file format.
//
//////////////////////////////////////////////////////////////////////////////

class SoTexture2;
class SbPList;
class SoSensor;

class SoV1Texture2 : public SoUpgrader {

    SO_NODE_HEADER(SoV1Texture2);

  public:
    enum Filter {			// Filter types
	POINT,
	BILINEAR,
	MIPMAP_POINT,
	MIPMAP_LINEAR,
	MIPMAP_BILINEAR,
	MIPMAP_TRILINEAR
    };

    enum Component {			// Component mask type
	NULL_COMPONENT,
	INTENSITY,
	TRANSPARENCY
    };

    enum Model {			// Texture model
	MODULATE,
	DECAL,
	BLEND
    };

    enum Wrap {				// Texture wrap type
	REPEAT,
	CLAMP
    };
    
    // Fields.
    SoSFBitMask		component;	// which components does this
					// texture define
    SoSFName		filename;
    SoSFEnum		minFilter;
    SoSFEnum		magFilter;
    SoSFEnum		wrapS;
    SoSFEnum		wrapT;
    SoSFEnum		model;
    SoSFColor		blendColor;
    SoSFVec2f		translation;
    SoSFVec2f		scaleFactor;
    SoSFFloat		rotation;
    SoSFVec2f		center;

    // Constructor
    SoV1Texture2();

  SoINTERNAL public:
    static void		initClass();

    virtual SoNode	*createNewNode();

  protected:
    virtual ~SoV1Texture2();

  private:
    SbBool findTexture2(SoTexture2 *&tex);
    SbBool matches(SoTexture2 *compare);

    static void nodeDeletedCB(void *node, SoSensor *sensor);

    static SbPList *textureList;
};

#endif /* _SO_V1TEXTURE2_ */
