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
 |	SoSFVec4f
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SF_VEC4F_
#define  _SO_SF_VEC4F_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoSFVec4f subclass of SoSField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSV4f
class SoSFVec4f : public SoSField {
    // Use standard field stuff
    SO_SFIELD_HEADER(SoSFVec4f, SbVec4f, const SbVec4f &);

  public:
    //
    // Some additional convenience functions:
    //

    // Set value from 4 floats
    // C-api: name=SetX_Y_Z_W
    void	setValue(float x, float y, float z, float w);

    // Set value from array of 4 floats
    // C-api: name=SetXYZW
    void	setValue(const float xyzw[4]);

  SoINTERNAL public:
    static void		initClass();
};

#endif /* _SO_SF_VEC4F_ */
