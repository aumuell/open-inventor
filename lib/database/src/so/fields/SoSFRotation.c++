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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoSFRotation
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoSFRotation.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFRotation class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_SFIELD_SOURCE(SoSFRotation, SbRotation, const SbRotation &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value from 4 separate floats describing a quaternion.
//    (Convenience function)
//
// Use: public

void
SoSFRotation::setValue(float q0, float q1, float q2, float q3)	// The 4 floats
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbRotation(q0, q1, q2, q3));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value from array of 4 floats describing a quaternion.
//    (Convenience function)
//
// Use: public

void
SoSFRotation::setValue(const float q[4])	// Array of values
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbRotation(q));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value from axis and angle. (Convenience function)
//
// Use: public

void
SoSFRotation::setValue(const SbVec3f &axis,	// The axis
		       float angle)		// The angle (in radians)
{
    setValue(SbRotation(axis, angle));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFRotation::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	axis;
    float	angle;

    if (! (in->read(axis[0]) &&
	   in->read(axis[1]) &&
	   in->read(axis[2]) &&
	   in->read(angle)))
	return FALSE;

    setValue(axis, angle);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFRotation::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	axis;
    float	angle;

    value.getValue(axis, angle);

    out->write(axis[0]);

    if (! out->isBinary())
	out->write(' ');

    out->write(axis[1]);

    if (! out->isBinary())
	out->write(' ');

    out->write(axis[2]);

    if (! out->isBinary()) {
	out->write(' ');
	out->write(' ');
    }

    out->write(angle);
}
