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
 |	SoMFRotation
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_ROTATION_
#define  _SO_MF_ROTATION_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFRotation subclass of SoMField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMRot
class SoMFRotation : public SoMField {
    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFRotation, SbRotation, const SbRotation &);

  public:

    //
    // Some additional convenience functions:
    //

    // Set values from array of arrays of 4 floats
    // C-api: name=SetQuats
    void	setValues(int start, int num, const float q[][4]);

    // Set one value from 4 floats
    // C-api: name=Set1Q_U_A_T
    void	set1Value(int index, float q0, float q1, float q2, float q3);

    // Set one value from 4 floats in array
    // C-api: name=Set1Quat
    void	set1Value(int index, const float q[4]);

    // Set one value from axis and angle
    // C-api: name=Set1AxisAngle
    void	set1Value(int index, const SbVec3f &axis, float angle);

    // Set to one value from 4 floats
    // C-api: name=SetQ_U_A_T
    void	setValue(float q0, float q1, float q2, float q3);

    // Set to one value from 4 floats in array
    // C-api: name=SetQuat
    void	setValue(const float q[4]);

    // Set to one value from axis and angle
    // C-api: name=SetAxisAngle
    void	setValue(const SbVec3f &axis, float angle);

  SoINTERNAL public:
    static void		initClass();
};

#endif /* _SO_MF_ROTATION_ */
