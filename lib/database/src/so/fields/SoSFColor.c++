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
 |	SoSFColor
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoSFColor.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFColor class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_SFIELD_SOURCE(SoSFColor, SbColor, const SbColor &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets RGB value from an SbVec3f. (Convenience function)
//
// Use: public

void
SoSFColor::setValue(const SbVec3f &vec)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbColor(vec.getValue()));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets RGB value from 3 separate floats. (Convenience function)
//
// Use: public

void
SoSFColor::setValue(float r, float g, float b)	// The 3 floats
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbColor(r, g, b));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets HSV value from 3 separate floats. (Convenience function)
//
// Use: public

void
SoSFColor::setHSVValue(float h, float s, float v)	// The 3 floats
//
////////////////////////////////////////////////////////////////////////
{
    SbColor	c;

    c.setHSVValue(h, s, v);
    setValue(c);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets RGB value from array of 3 floats. (Convenience function)
//
// Use: public

void
SoSFColor::setValue(const float rgb[3])		// Array of values
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbColor(rgb));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets HSV value from array of 3 floats. (Convenience function)
//
// Use: public

void
SoSFColor::setHSVValue(const float hsv[3])		// Array of values
//
////////////////////////////////////////////////////////////////////////
{
    SbColor	c;

    c.setHSVValue(hsv);
    setValue(c);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFColor::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    return (in->read(value[0]) &&
	    in->read(value[1]) &&
	    in->read(value[2]));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFColor::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(value[0]);

    if (! out->isBinary())
	out->write(' ');

    out->write(value[1]);

    if (! out->isBinary())
	out->write(' ');

    out->write(value[2]);
}
