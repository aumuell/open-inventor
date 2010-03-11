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
 |	SoMFRotation
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoMFRotation.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFRotation class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_MFIELD_SOURCE_MALLOC(SoMFRotation, SbRotation, const SbRotation &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets values from array of arrays of 4 floats. This can be useful
//    in some applications that have quaternions stored in this manner and
//    want to keep them that way for efficiency.
//
// Use: public

void
SoMFRotation::setValues(int start,			// Starting index
		     int num,			// Number of values to set
		     const float q[][4])	// Array of quaternion values
//
////////////////////////////////////////////////////////////////////////
{
    int	newNum = start + num;
    int	i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	values[start + i].setValue(q[i]);

    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one quaternion value from 4 separate floats. (Convenience function)
//
// Use: public

void
SoMFRotation::set1Value(int index, float q0, float q1, float q2, float q3)
//
////////////////////////////////////////////////////////////////////////
{
    set1Value(index, SbRotation(q0, q1, q2, q3));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one quaternion value from an array of 4 floats. (Convenience function)
//
// Use: public

void
SoMFRotation::set1Value(int index, const float q[4])
//
////////////////////////////////////////////////////////////////////////
{
    set1Value(index, SbRotation(q));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one rotation value from an axis and angle. (Convenience function)
//
// Use: public

void
SoMFRotation::set1Value(int index, const SbVec3f &axis, float angle)
//
////////////////////////////////////////////////////////////////////////
{
    set1Value(index, SbRotation(axis, angle));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to one quaternion value from 4 separate floats. (Convenience function)
//
// Use: public

void
SoMFRotation::setValue(float q0, float q1, float q2, float q3)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbRotation(q0, q1, q2, q3));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to one quaternion value from an array of 4 floats. (Convenience function)
//
// Use: public

void
SoMFRotation::setValue(const float q[4])
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbRotation(q));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to one rotation value from an axis and angle. (Convenience function)
//
// Use: public

void
SoMFRotation::setValue(const SbVec3f &axis, float angle)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbRotation(axis, angle));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFRotation::read1Value(SoInput *in, int index)
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

    set1Value(index, axis, angle);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes one (indexed) value to file.
//
// Use: private

void
SoMFRotation::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	axis;
    float	angle;

    values[index].getValue(axis, angle);

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
