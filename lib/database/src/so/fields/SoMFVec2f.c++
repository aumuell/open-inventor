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
 |	SoMFVec2f
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoMFVec2f.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFVec2f class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_MFIELD_SOURCE_MALLOC(SoMFVec2f, SbVec2f, const SbVec2f &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets values from array of arrays of 2 floats. This can be useful
//    in some applications that have vectors stored in this manner and
//    want to keep them that way for efficiency.
//
// Use: public

void
SoMFVec2f::setValues(int start,			// Starting index
		     int num,			// Number of values to set
		     const float xy[][2])	// Array of vector values
//
////////////////////////////////////////////////////////////////////////
{
    int	newNum = start + num;
    int	i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	values[start + i].setValue(xy[i]);

    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one vector value from 2 separate floats. (Convenience function)
//
// Use: public

void
SoMFVec2f::set1Value(int index, float x, float y)
//
////////////////////////////////////////////////////////////////////////
{
    set1Value(index, SbVec2f(x, y));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one vector value from an array of 2 floats. (Convenience function)
//
// Use: public

void
SoMFVec2f::set1Value(int index, const float xy[2])
//
////////////////////////////////////////////////////////////////////////
{
    set1Value(index, SbVec2f(xy));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to one vector value from 2 separate floats. (Convenience function)
//
// Use: public

void
SoMFVec2f::setValue(float x, float y)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbVec2f(x, y));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to one vector value from array of 2 floats. (Convenience function)
//
// Use: public

void
SoMFVec2f::setValue(const float xy[2])
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbVec2f(xy));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFVec2f::read1Value(SoInput *in, int index)
//
////////////////////////////////////////////////////////////////////////
{
    return (in->read(values[index][0]) &&
	    in->read(values[index][1]));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes one (indexed) value to file.
//
// Use: private

void
SoMFVec2f::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(values[index][0]);

    if (! out->isBinary())
	out->write(' ');

    out->write(values[index][1]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes array of binary values to file as one chunk.
//
// Use: private

void
SoMFVec2f::writeBinaryValues(SoOutput *out) const

//
////////////////////////////////////////////////////////////////////////
{
    out->writeBinaryArray((float *) values, 2 * num);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads array of binary values from file as one chunk.
//
// Use: private

SbBool
SoMFVec2f::readBinaryValues(SoInput *in, int numToRead)
//
////////////////////////////////////////////////////////////////////////
{
    return (in->readBinaryArray((float *) values, 2 * numToRead));
}
