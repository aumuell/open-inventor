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
 |	SoMFVec3f
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_VEC3F_
#define  _SO_MF_VEC3F_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFVec3f subclass of SoMField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMV3f
class SoMFVec3f : public SoMField {
    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFVec3f, SbVec3f, const SbVec3f &);

  public:

    //
    // Some additional convenience functions:
    //

    // Set values from array of arrays of 3 floats
    // C-api: name=SetXYZs
    void	setValues(int start, int num, const float xyz[][3]);

    // Set one value from 3 floats
	// C-api: name=Set1X_Y_Z
    void	set1Value(int index, float x, float y, float z);

    // Set one value from 3 floats in array
	// C-api: name=Set1XYZ
    void	set1Value(int index, const float xyz[3]);

    // Set to one value from 3 floats
	// C-api: name=SetX_Y_Z
    void	setValue(float x, float y, float z);

    // Set to one value from 3 floats in array
	// C-api: name=SetXYZ
    void	setValue(const float xyz[3]);

  SoINTERNAL public:
    static void		initClass();

  private:

    // Write the values out as a block of data
    virtual void	writeBinaryValues(SoOutput *out) const;
    virtual SbBool	readBinaryValues(SoInput *in, int numToRead);
};

#endif /* _SO_MF_VEC3F_ */
