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
 |      SoTransformVec3f
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoTransformVec3f.h>
#include "SoEngineUtil.h"

SO_ENGINE_SOURCE(SoTransformVec3f);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTransformVec3f::SoTransformVec3f()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoTransformVec3f);
    SO_ENGINE_ADD_INPUT(vector,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(matrix,	  (SbMatrix::identity()));
    SO_ENGINE_ADD_OUTPUT(point, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(direction, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(normalDirection, SoMFVec3f);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoTransformVec3f::~SoTransformVec3f()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoTransformVec3f::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    int	nvector = vector.getNum();
    int nmatrix = matrix.getNum();
    int nout = max(nvector,nmatrix);
    SO_ENGINE_OUTPUT(point, SoMFVec3f, setNum(nout));
    SO_ENGINE_OUTPUT(direction, SoMFVec3f, setNum(nout));
    SO_ENGINE_OUTPUT(normalDirection, SoMFVec3f, setNum(nout));

    for (int i=0; i<nout; i++) {
	const SbVec3f &v = vector[clamp(i,nvector)];
	const SbMatrix &m = matrix[clamp(i,nmatrix)];
	SbVec3f p, d;

	if (point.getNumConnections()) {
	    m.multVecMatrix(v, p);
	    SO_ENGINE_OUTPUT(point, SoMFVec3f, set1Value(i, p));
	}

	if (direction.getNumConnections() ||
	    normalDirection.getNumConnections()) {
	    m.multDirMatrix(v, d);
	}

	if (direction.getNumConnections())
	    SO_ENGINE_OUTPUT(direction, SoMFVec3f, set1Value(i, d));

	if (normalDirection.getNumConnections()) {
	    d.normalize();
	    SO_ENGINE_OUTPUT(normalDirection, SoMFVec3f, set1Value(i, d));
	}
    }
}
