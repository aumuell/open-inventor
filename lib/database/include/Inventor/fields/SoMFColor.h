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
 |	SoMFColor
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_COLOR_
#define  _SO_MF_COLOR_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbColor.h>
#include <Inventor/fields/SoMFVec3f.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFColor subclass of SoMField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMCol
class SoMFColor : public SoMField {
    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFColor, SbColor, const SbColor &);

  public:
    //
    // Additional convenience functions
    //

    // Set RGB/HSV values from array of arrays of 3 floats
    // C-api: name=SetRGBs
    void		setValues(int start, int num, const float rgb[][3]);
    // C-api: name=SetHSVs
    void		setHSVValues(int start, int num, const float hsv[][3]);

    // Set to one RGB color from SbVec3f, 3 floats, or array of 3 floats
    // C-api: name=SetV3f
    void		setValue(const SbVec3f &vec);
    // C-api: name=SetR_G_B
    void		setValue(float r, float g, float b);
    // C-api: name=SetRGB
    void		setValue(const float rgb[3]);

    // Set to one HSV color from 3 floats or array of 3 floats
    // C-api: name=SetH_S_V
    void		setHSVValue(float h, float s, float v);
    // C-api: name=SetHSV
    void		setHSVValue(const float hsv[3]);

    // Set one of N RGB colors from SbVec3f, 3 floats, or array of 3 floats
    // C-api: name=Set1V3f
    void		set1Value(int index, const SbVec3f &vec);
    // C-api: name=Set1R_G_B
    void		set1Value(int index, float r, float g, float b);
    // C-api: name=Set1RGB
    void		set1Value(int index, const float rgb[3]);

    // Set1 one of N HSV colors from 3 floats or array of 3 floats
    // C-api: name=Set1H_S_V
    void		set1HSVValue(int index, float h, float s, float v);
    // C-api: name=Set1HSV
    void		set1HSVValue(int index, const float hsv[3]);

  SoINTERNAL public:
    static void		initClass();

  private:
    virtual void	writeBinaryValues(SoOutput *out) const;
    virtual SbBool	readBinaryValues(SoInput *in, int numToRead);
};

#endif /* _SO_MF_COLOR_ */
