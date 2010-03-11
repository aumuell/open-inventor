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
 |      SoComposeVec3f
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoCompose.h>
#include "SoEngineUtil.h"

//////////////////
//
// Utility macro defines basic source for composition/decomposition
// engines
//
#define SO_COMPOSE__SOURCE(Name)			\
    SO_ENGINE_SOURCE(Name);				\
    Name::~Name() { }

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeVec2f
//

SO_COMPOSE__SOURCE(SoComposeVec2f);

SoComposeVec2f::SoComposeVec2f()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeVec2f);
    SO_ENGINE_ADD_INPUT(x,	  (0.0));
    SO_ENGINE_ADD_INPUT(y,	  (0.0));
    SO_ENGINE_ADD_OUTPUT(vector, SoMFVec2f);
    isBuiltIn = TRUE;
}

void
SoComposeVec2f::evaluate()
{
    int	nx = x.getNum();
    int ny = y.getNum();
    int nout = max(nx,ny);
    SO_ENGINE_OUTPUT(vector, SoMFVec2f, setNum(nout));
    for (int i=0; i<nout; i++) {
	float vx = x[clamp(i,nx)];
	float vy = y[clamp(i,ny)];
	SO_ENGINE_OUTPUT(vector, SoMFVec2f, set1Value(i, vx, vy));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeVec3f
//

SO_COMPOSE__SOURCE(SoComposeVec3f);

SoComposeVec3f::SoComposeVec3f()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeVec3f);
    SO_ENGINE_ADD_INPUT(x,	  (0.0));
    SO_ENGINE_ADD_INPUT(y,	  (0.0));
    SO_ENGINE_ADD_INPUT(z,	  (0.0));
    SO_ENGINE_ADD_OUTPUT(vector, SoMFVec3f);
    isBuiltIn = TRUE;
}

void
SoComposeVec3f::evaluate()
{
    int	nx = x.getNum();
    int ny = y.getNum();
    int nz = z.getNum();
    int nout=max(nx,ny,nz);
    SO_ENGINE_OUTPUT(vector, SoMFVec3f, setNum(nout));
    for (int i=0; i<nout; i++) {
	float vx = x[clamp(i,nx)];
	float vy = y[clamp(i,ny)];
	float vz = z[clamp(i,nz)];
	SO_ENGINE_OUTPUT(vector, SoMFVec3f, set1Value(i, vx, vy, vz));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeVec4f
//

SO_COMPOSE__SOURCE(SoComposeVec4f);

SoComposeVec4f::SoComposeVec4f()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeVec4f);
    SO_ENGINE_ADD_INPUT(x,	  (0.0));
    SO_ENGINE_ADD_INPUT(y,	  (0.0));
    SO_ENGINE_ADD_INPUT(z,	  (0.0));
    SO_ENGINE_ADD_INPUT(w,	  (0.0));
    SO_ENGINE_ADD_OUTPUT(vector, SoMFVec4f);
    isBuiltIn = TRUE;
}

void
SoComposeVec4f::evaluate()
{
    int	nx = x.getNum();
    int ny = y.getNum();
    int nz = z.getNum();
    int nw = w.getNum();
    int nout=max(nx,ny,nz,nw);
    SO_ENGINE_OUTPUT(vector, SoMFVec4f, setNum(nout));
    for (int i=0; i<nout; i++) {
	float vx = x[clamp(i,nx)];
	float vy = y[clamp(i,ny)];
	float vz = z[clamp(i,nz)];
	float vw = w[clamp(i,nw)];
	SO_ENGINE_OUTPUT(vector, SoMFVec4f, set1Value(i, vx, vy, vz, vw));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoDecomposeVec2f
//

SO_COMPOSE__SOURCE(SoDecomposeVec2f);

SoDecomposeVec2f::SoDecomposeVec2f()
{
    SO_ENGINE_CONSTRUCTOR(SoDecomposeVec2f);
    SO_ENGINE_ADD_INPUT(vector,	  (SbVec2f(0,0)));
    SO_ENGINE_ADD_OUTPUT(x, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(y, SoMFFloat);
    isBuiltIn = TRUE;
}

void
SoDecomposeVec2f::evaluate()
{
    int nout=vector.getNum();
    SO_ENGINE_OUTPUT(x, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(y, SoMFFloat, setNum(nout));
    for (int i=0; i<nout; i++) {
	SO_ENGINE_OUTPUT(x, SoMFFloat, set1Value(i,vector[i][0]));
	SO_ENGINE_OUTPUT(y, SoMFFloat, set1Value(i,vector[i][1]));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoDecomposeVec3f
//

SO_COMPOSE__SOURCE(SoDecomposeVec3f);

SoDecomposeVec3f::SoDecomposeVec3f()
{
    SO_ENGINE_CONSTRUCTOR(SoDecomposeVec3f);
    SO_ENGINE_ADD_INPUT(vector,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_OUTPUT(x, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(y, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(z, SoMFFloat);
    isBuiltIn = TRUE;
}

void
SoDecomposeVec3f::evaluate()
{
    int nout=vector.getNum();
    SO_ENGINE_OUTPUT(x, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(y, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(z, SoMFFloat, setNum(nout));
    for (int i=0; i<nout; i++) {
	SO_ENGINE_OUTPUT(x, SoMFFloat, set1Value(i,vector[i][0]));
	SO_ENGINE_OUTPUT(y, SoMFFloat, set1Value(i,vector[i][1]));
	SO_ENGINE_OUTPUT(z, SoMFFloat, set1Value(i,vector[i][2]));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoDecomposeVec4f
//

SO_COMPOSE__SOURCE(SoDecomposeVec4f);

SoDecomposeVec4f::SoDecomposeVec4f()
{
    SO_ENGINE_CONSTRUCTOR(SoDecomposeVec4f);
    SO_ENGINE_ADD_INPUT(vector,	  (SbVec4f(0,0,0,0)));
    SO_ENGINE_ADD_OUTPUT(x, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(y, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(z, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(w, SoMFFloat);
    isBuiltIn = TRUE;
}

void
SoDecomposeVec4f::evaluate()
{
    int nout=vector.getNum();
    SO_ENGINE_OUTPUT(x, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(y, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(z, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(w, SoMFFloat, setNum(nout));
    for (int i=0; i<nout; i++) {
	SO_ENGINE_OUTPUT(x, SoMFFloat, set1Value(i,vector[i][0]));
	SO_ENGINE_OUTPUT(y, SoMFFloat, set1Value(i,vector[i][1]));
	SO_ENGINE_OUTPUT(z, SoMFFloat, set1Value(i,vector[i][2]));
	SO_ENGINE_OUTPUT(w, SoMFFloat, set1Value(i,vector[i][3]));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeRotation
//

SO_COMPOSE__SOURCE(SoComposeRotation);

SoComposeRotation::SoComposeRotation()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeRotation);
    SO_ENGINE_ADD_INPUT(axis,	  (SbVec3f(0,0,1)));
    SO_ENGINE_ADD_INPUT(angle,	  (0.0));
    SO_ENGINE_ADD_OUTPUT(rotation, SoMFRotation);
    isBuiltIn = TRUE;
}

void
SoComposeRotation::evaluate()
{
    int	naxis = axis.getNum();
    int nangle = angle.getNum();
    int nout=max(naxis,nangle);
    SO_ENGINE_OUTPUT(rotation, SoMFRotation, setNum(nout));
    for (int i=0; i<nout; i++) {
	SbVec3f vaxis = axis[clamp(i,naxis)];
	float	vangle = angle[clamp(i,nangle)];
	SO_ENGINE_OUTPUT(rotation, SoMFRotation, set1Value(i, vaxis, vangle));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeRotationFromTo
//

SO_COMPOSE__SOURCE(SoComposeRotationFromTo);

SoComposeRotationFromTo::SoComposeRotationFromTo()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeRotationFromTo);
    SO_ENGINE_ADD_INPUT(from,	  (SbVec3f(0,0,1)));
    SO_ENGINE_ADD_INPUT(to,	  (SbVec3f(0,0,1)));
    SO_ENGINE_ADD_OUTPUT(rotation, SoMFRotation);
    isBuiltIn = TRUE;
}

void
SoComposeRotationFromTo::evaluate()
{
    int	nfrom = from.getNum();
    int	nto = to.getNum();
    int nout=max(nfrom,nto);
    SO_ENGINE_OUTPUT(rotation, SoMFRotation, setNum(nout));
    for (int i=0; i<nout; i++) {
	SbVec3f vfrom = from[clamp(i,nfrom)];
	SbVec3f vto = to[clamp(i,nto)];
	SO_ENGINE_OUTPUT(rotation, SoMFRotation, set1Value(i, SbRotation(vfrom, vto)));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoDecomposeRotation
//

SO_COMPOSE__SOURCE(SoDecomposeRotation);

SoDecomposeRotation::SoDecomposeRotation()
{
    SO_ENGINE_CONSTRUCTOR(SoDecomposeRotation);
    SO_ENGINE_ADD_INPUT(rotation,	  (SbRotation::identity()));
    SO_ENGINE_ADD_OUTPUT(axis, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(angle, SoMFFloat);
    isBuiltIn = TRUE;
}

void
SoDecomposeRotation::evaluate()
{
    int nout=rotation.getNum();
    SO_ENGINE_OUTPUT(axis, SoMFVec3f, setNum(nout));
    SO_ENGINE_OUTPUT(angle, SoMFFloat, setNum(nout));
    for (int i=0; i<nout; i++) {
	SbVec3f vaxis;
	float	vangle;
	rotation[i].getValue(vaxis, vangle);
	SO_ENGINE_OUTPUT(axis, SoMFVec3f, set1Value(i,vaxis));
	SO_ENGINE_OUTPUT(angle, SoMFFloat, set1Value(i,vangle));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoComposeMatrix
//

SO_COMPOSE__SOURCE(SoComposeMatrix);

SoComposeMatrix::SoComposeMatrix()
{
    SO_ENGINE_CONSTRUCTOR(SoComposeMatrix);
    SO_ENGINE_ADD_INPUT(translation,	(SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(rotation,	(SbRotation::identity()));
    SO_ENGINE_ADD_INPUT(scaleFactor,	(SbVec3f(1,1,1)));
    SO_ENGINE_ADD_INPUT(scaleOrientation,(SbRotation::identity()));
    SO_ENGINE_ADD_INPUT(center,		(SbVec3f(0,0,0)));
    SO_ENGINE_ADD_OUTPUT(matrix, SoMFMatrix);
    isBuiltIn = TRUE;
}

void
SoComposeMatrix::evaluate()
{
    int ntranslation = translation.getNum();
    int nrotation = rotation.getNum();
    int nscaleFactor = scaleFactor.getNum();
    int nscaleOrientation = scaleOrientation.getNum();
    int ncenter = center.getNum();
    int nout=max(ntranslation,nrotation,nscaleFactor,nscaleOrientation,ncenter);
    SO_ENGINE_OUTPUT(matrix, SoMFMatrix, setNum(nout));
    for (int i=0; i<nout; i++) {
	SbVec3f vtrans = translation[clamp(i,ntranslation)];
	SbRotation vrot = rotation[clamp(i,nrotation)];
	SbVec3f vscale = scaleFactor[clamp(i,nscaleFactor)];
	SbRotation vscaleO = scaleOrientation[clamp(i,nscaleOrientation)];
	SbVec3f vcenter = center[clamp(i,ncenter)];
	SbMatrix m;
	m.setTransform(vtrans, vrot, vscale, vscaleO, vcenter);
	SO_ENGINE_OUTPUT(matrix, SoMFMatrix, set1Value(i, m));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Source for SoDecomposeMatrix
//

SO_COMPOSE__SOURCE(SoDecomposeMatrix);

SoDecomposeMatrix::SoDecomposeMatrix()
{
    SO_ENGINE_CONSTRUCTOR(SoDecomposeMatrix);
    SO_ENGINE_ADD_INPUT(matrix,	  (SbMatrix::identity()));
    SO_ENGINE_ADD_INPUT(center,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_OUTPUT(translation, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(rotation, SoMFRotation);
    SO_ENGINE_ADD_OUTPUT(scaleFactor, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(scaleOrientation, SoMFRotation);
    isBuiltIn = TRUE;
}

void
SoDecomposeMatrix::evaluate()
{
    int nmatrix = matrix.getNum();
    int ncenter = center.getNum();
    int nout=max(nmatrix,ncenter);
    SO_ENGINE_OUTPUT(translation, SoMFVec3f, setNum(nout));
    SO_ENGINE_OUTPUT(rotation, SoMFRotation, setNum(nout));
    SO_ENGINE_OUTPUT(scaleFactor, SoMFVec3f, setNum(nout));
    SO_ENGINE_OUTPUT(scaleOrientation, SoMFRotation, setNum(nout));
    for (int i=0; i<nout; i++) {
	SbVec3f trans;
	SbRotation rot;
	SbVec3f	scale;
	SbRotation scaleO;
	SbVec3f vcenter = center[clamp(i,ncenter)];
	SbMatrix vmatrix = matrix[clamp(i,nmatrix)];
	vmatrix.getTransform(trans, rot, scale, scaleO, vcenter);
	SO_ENGINE_OUTPUT(translation, SoMFVec3f, set1Value(i, trans));
	SO_ENGINE_OUTPUT(rotation, SoMFRotation, set1Value(i, rot));
	SO_ENGINE_OUTPUT(scaleFactor, SoMFVec3f, set1Value(i, scale));
	SO_ENGINE_OUTPUT(scaleOrientation, SoMFRotation, set1Value(i, scaleO));
    }
}
