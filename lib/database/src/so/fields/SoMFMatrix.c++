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
 |	SoMFMatrix
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoMFMatrix.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFMatrix class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_MFIELD_SOURCE_MALLOC(SoMFMatrix, SbMatrix, const SbMatrix &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets one matrix value from 16 separate floats. (Convenience function)
//
// Use: public

void
SoMFMatrix::setValue(float a11, float a12, float a13, float a14,
		     float a21, float a22, float a23, float a24,
		     float a31, float a32, float a33, float a34,
		     float a41, float a42, float a43, float a44)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbMatrix(a11, a12, a13, a14,
		      a21, a22, a23, a24,
		      a31, a32, a33, a34,
		      a41, a42, a43, a44));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFMatrix::read1Value(SoInput *in, int index)
//
////////////////////////////////////////////////////////////////////////
{
    return  in->read(values[index][0][0]) && in->read(values[index][0][1])
	 && in->read(values[index][0][2]) && in->read(values[index][0][3])
	 && in->read(values[index][1][0]) && in->read(values[index][1][1])
	 && in->read(values[index][1][2]) && in->read(values[index][1][3])
	 && in->read(values[index][2][0]) && in->read(values[index][2][1])
	 && in->read(values[index][2][2]) && in->read(values[index][2][3])
	 && in->read(values[index][3][0]) && in->read(values[index][3][1])
	 && in->read(values[index][3][2]) && in->read(values[index][3][3]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes one (indexed) value to file.
//
// Use: private

void
SoMFMatrix::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    if (! out->isBinary())
	out->incrementIndent(2);

    for (i = 0; i < 4; i++) {
	if (i > 0 && ! out->isBinary())
	    out->indent();

	out->write(values[index][i][0]);

	if (! out->isBinary())
	    out->write(' ');

	out->write(values[index][i][1]);

	if (! out->isBinary())
	    out->write(' ');

	out->write(values[index][i][2]);

	if (! out->isBinary())
	    out->write(' ');

	out->write(values[index][i][3]);

	if (i != 3 && ! out->isBinary())
	    out->write('\n');
    }

    if (! out->isBinary())
	out->decrementIndent(2);
}
