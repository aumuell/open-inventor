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
 |	SoMFVec2f
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_VEC2F_
#define  _SO_MF_VEC2F_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFVec2f subclass of SoMField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMV2f
class SoMFVec2f : public SoMField {
    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFVec2f, SbVec2f, const SbVec2f &);

  public:
    //
    // Some additional convenience functions:
    //

    // Set values from array of arrays of 2 floats
    // C-api: name=SetXYs
    void	setValues(int start, int num, const float xy[][2]);

    // Set one value from 2 floats
	// C-api: name=Set1X_Y
    void	set1Value(int index, float x, float y);

    // Set one value from 2 floats in array
	// C-api: name=Set1XY
    void	set1Value(int index, const float xy[2]);

    // Set to one value from 2 floats
	// C-api: name=SetX_Y
    void	setValue(float x, float y);

    // Set to one value from 2 floats in array
	// C-api: name=SetXY
    void	setValue(const float xy[2]);

  SoINTERNAL public:
    static void	initClass();

  private:
    virtual void	writeBinaryValues(SoOutput *out) const;
    virtual SbBool	readBinaryValues(SoInput *in, int numToRead);
};

#endif /* _SO_MF_VEC2F_ */
