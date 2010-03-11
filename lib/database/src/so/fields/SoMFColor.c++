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
 |	SoMFColor
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoMFColor.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFColor class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_MFIELD_SOURCE_MALLOC(SoMFColor, SbColor, const SbColor &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets RGB values from array of arrays of 3 floats. This can be useful
//    in some applications that have colors stored in this manner and
//    want to keep them that way for efficiency.
//
// Use: public

void
SoMFColor::setValues(int start,			// Starting index
		     int num,			// Number of values to set
		     const float rgb[][3])	// Array of RGB values
//
////////////////////////////////////////////////////////////////////////
{
    int	newNum = start + num;
    int	i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	values[start + i].setValue(rgb[i]);

    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets HSV values from array of arrays of 3 floats. This can be useful
//    in some applications that have colors stored in this manner and
//    want to keep them that way for efficiency.
//
// Use: public

void
SoMFColor::setHSVValues(int start,		// Starting index
			int num,		// Number of values to set
			const float hsv[][3])	// Array of HSV values
//
////////////////////////////////////////////////////////////////////////
{
    int	newNum = start + num;
    int	i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	values[start + i].setHSVValue(hsv[i]);

    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    These are all convenience functions that set the field to
//    contain one and only one value, given that value in various
//    forms, both RGB and HSV.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

void
SoMFColor::setValue(const SbVec3f &vec)
{
    setValue(SbColor(vec.getValue()));
}

void
SoMFColor::setValue(float r, float g, float b)
{
    setValue(SbColor(r, g, b));
}

void
SoMFColor::setValue(const float rgb[3])
{
    setValue(SbColor(rgb));
}

void
SoMFColor::setHSVValue(float h, float s, float v)
{
    SbColor	c;

    c.setHSVValue(h, s, v);
    setValue(c);
}

void
SoMFColor::setHSVValue(const float hsv[3])
{
    SbColor	c;

    c.setHSVValue(hsv);
    setValue(c);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    These are all convenience functions that set one value of the
//    field to a given value, which is specified in various forms,
//    both RGB and HSV.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

void
SoMFColor::set1Value(int index, const SbVec3f &vec)
{
    set1Value(index, SbColor(vec.getValue()));
}

void
SoMFColor::set1Value(int index, float r, float g, float b)
{
    set1Value(index, SbColor(r, g, b));
}

void
SoMFColor::set1Value(int index, const float rgb[3])
{
    set1Value(index, SbColor(rgb));
}

void
SoMFColor::set1HSVValue(int index, float h, float s, float v)
{
    SbColor	c;

    c.setHSVValue(h, s, v);
    set1Value(index, c);
}

void
SoMFColor::set1HSVValue(int index, const float hsv[3])
{
    SbColor	c;

    c.setHSVValue(hsv);
    set1Value(index, c);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFColor::read1Value(SoInput *in, int index)
//
////////////////////////////////////////////////////////////////////////
{
    return (in->read(values[index][0]) &&
	    in->read(values[index][1]) &&
	    in->read(values[index][2]));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes one (indexed) value to file.
//
// Use: private

void
SoMFColor::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(values[index][0]);

    if (! out->isBinary())
	out->write(' ');

    out->write(values[index][1]);

    if (! out->isBinary())
	out->write(' ');

    out->write(values[index][2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes array of binary values to file as one chunk.
//
// Use: private

void
SoMFColor::writeBinaryValues(SoOutput *out) const

//
////////////////////////////////////////////////////////////////////////
{
    out->writeBinaryArray((float *) values, 3 * num);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads array of binary values from file as one chunk.
//
// Use: private

SbBool
SoMFColor::readBinaryValues(SoInput *in, int numToRead)
//
////////////////////////////////////////////////////////////////////////
{
    return (in->readBinaryArray((float *) values, 3 * numToRead));
}
