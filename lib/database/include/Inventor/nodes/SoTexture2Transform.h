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
 |	This file defines the SoTexture2Transform node class.
 |
 |   Author(s)		: Paul S. Strauss, Thaddeus Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_2_TRANSFORM_
#define  _SO_TEXTURE_2_TRANSFORM_

#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTexture2Transform
//
//  2D Texture transformation node.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoTex2Xf
// C-api: public= translation, rotation, scaleFactor, center
class SoTexture2Transform : public SoNode {

    SO_NODE_HEADER(SoTexture2Transform);

  public:
    // Fields
    SoSFVec2f		translation;	// Translation vector
    SoSFFloat		rotation;	// Rotation
    SoSFVec2f		scaleFactor;	// Scale factors
    SoSFVec2f		center;	        // Center point for scale and rotate

    // Constructor
    SoTexture2Transform();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoTexture2Transform();
};

#endif /* _SO_TEXTURE_2_TRANSFORM_ */
