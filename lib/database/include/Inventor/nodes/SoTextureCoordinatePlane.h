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
 |	The TextureCoordinatePlane.  This texture coordinate function
 |      maps object space coordinates (xyz) to texture space (st)
 |      coordinates by projecting the object space coordinates onto
 |      a plane.
 |
 |   Author(s)		: Thad Beier, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_COORDINATE_PLANE_
#define  _SO_TEXTURE_COORDINATE_PLANE_

#include <Inventor/nodes/SoTextureCoordinateFunction.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureCoordinatePlane
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoTexPln
// C-api: public=directionS, directionT
class SoTextureCoordinatePlane : public SoTextureCoordinateFunction {

    SO_NODE_HEADER(SoTextureCoordinatePlane);

  public:
    // Fields
    SoSFVec3f		directionS; // S coordinates projection direction
    SoSFVec3f		directionT; // T coordinates projection direction

    // Constructor
    SoTextureCoordinatePlane();

  SoEXTENDER public:
    void		GLRender(SoGLRenderAction *action);
    void		pick(SoPickAction *action);
    void		callback(SoCallbackAction *action);
    
    void		doAction(SoAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoTextureCoordinatePlane();

  private:
    // Callback used internally to project a point:
    static const SbVec4f &valueCallback(void *instance,
	const SbVec3f &position, const SbVec3f &);

    // Callback used internally to send GL texgen commands:
    static void doTexgen(void *);
};

#endif /* _SO_TEXTURE_COORDINATE_PLANE_ */
