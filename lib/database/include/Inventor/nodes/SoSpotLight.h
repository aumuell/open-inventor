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
 |	This file defines the SoSpotLight node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SPOT_LIGHT_
#define  _SO_SPOT_LIGHT_

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoLight.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSpotLight
//
//  Spot light source node. A spot light illuminates from a point in
//  space along a primary direction. The illumination is within a cone
//  whose angle is given by the cutOffAngle field. The intensity of
//  the source may vary as the illumination direction diverges from
//  the primary direction, as specified in the dropOffRate field.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSpotLt
// C-api: public=on, intensity, color
// C-api: public=location, direction, dropOffRate, cutOffAngle
class SoSpotLight : public SoLight {

    SO_NODE_HEADER(SoSpotLight);

  public:
    // Fields (in addition to those in SoLight):
    SoSFVec3f	location;	// Source location
    SoSFVec3f	direction;	// Primary direction of illumination
    SoSFFloat	dropOffRate;	// Rate of intensity drop-off from primary
				// direction: 0 = constant intensity,
				// 1 = sharp drop-off
    SoSFFloat	cutOffAngle;	// Angle (in radians) outside of which
				// intensity is zero, measured from
				// edge of cone to other edge

    // Constructor
    SoSpotLight();

  SoEXTENDER public:
    // Creates a light source during rendering
    virtual void	GLRender(SoGLRenderAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoSpotLight();
};

#endif /* _SO_SPOT_LIGHT_ */
