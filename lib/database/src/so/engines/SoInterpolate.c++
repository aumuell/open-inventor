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
 |      SoInterpolate
 |	SoInterpolateFloat
 |	SoInterpolateRotation
 |	SoInterpolateVec2f
 |	SoInterpolateVec3f
 |	SoInterpolateVec4f
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoInterpolate.h>

////////////////
//
// miscellaneous utilities
//

/*
** Source for the SoInterpolate base class
*/

SO_ENGINE_ABSTRACT_SOURCE(SoInterpolate);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoInterpolate::SoInterpolate()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoInterpolate);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoInterpolate::~SoInterpolate()
//
////////////////////////////////////////////////////////////////////////
{
}

/*
** Source for the builtin field type interpolaters
*/

SO_INTERPOLATE_SOURCE(SoInterpolateFloat,
	    SoMFFloat, float, (0), (1), ((1-a)*v0)+(a*v1));

SO_INTERPOLATE_SOURCE(SoInterpolateRotation,
	    SoMFRotation, SbRotation,
	    (SbRotation::identity()), (SbRotation::identity()),
	    SbRotation::slerp(v0,v1,a));

SO_INTERPOLATE_SOURCE(SoInterpolateVec2f, SoMFVec2f, 
		    SbVec2f, (SbVec2f(0,0)), (SbVec2f(0,0)),
		    ((1-a)*v0)+(a*v1));

SO_INTERPOLATE_SOURCE(SoInterpolateVec3f, SoMFVec3f, 
		    SbVec3f, (SbVec3f(0,0,0)), (SbVec3f(0,0,0)),
		    ((1-a)*v0)+(a*v1));

SO_INTERPOLATE_SOURCE(SoInterpolateVec4f, SoMFVec4f, 
		    SbVec4f, (SbVec4f(0,0,0,0)), (SbVec4f(0,0,0,0)),
		    ((1-a)*v0)+(a*v1));


