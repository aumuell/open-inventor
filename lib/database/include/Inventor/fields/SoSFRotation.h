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
 |   Classes:
 |	SoSFRotation
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SF_ROTATION_
#define  _SO_SF_ROTATION_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoSFRotation subclass of SoSField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSRot
class SoSFRotation : public SoSField {
    // Use standard field stuff
    SO_SFIELD_HEADER(SoSFRotation, SbRotation, const SbRotation &);

  public:
    //
    // Some additional convenience functions:
    //

    // Return value as axis and angle
    // C-api: name=GetAxisAngle
    void	getValue(SbVec3f &axis, float &angle) const
	{ evaluate(); value.getValue(axis, angle); }

    // Set value from 4 floats defining rotation quaternion
    // C-api: name=SetQ_U_A_T
    void	setValue(float q0, float q1, float q2, float q3);
    
    // Set value from array of 4 floats defining rotation quaternion
    // C-api: name=SetQuat
    void	setValue(const float q[4]);

    // Set value from axis and angle in radians
    // C-api: name=SetAxisAngle
    void	setValue(const SbVec3f &axis, float angle);

  SoINTERNAL public:
    static void		initClass();
};

#endif /* _SO_SF_ROTATION_ */
