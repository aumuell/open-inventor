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
 |      SoBoolOperation
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoBoolOperation.h>
#include "SoEngineUtil.h"

SO_ENGINE_SOURCE(SoBoolOperation);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoBoolOperation::SoBoolOperation()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoBoolOperation);
    SO_ENGINE_ADD_INPUT(a,	  (FALSE));
    SO_ENGINE_ADD_INPUT(b,	  (FALSE));
    SO_ENGINE_ADD_INPUT(operation,	  (A));
    SO_ENGINE_ADD_OUTPUT(output, SoMFBool);
    SO_ENGINE_ADD_OUTPUT(inverse, SoMFBool);

    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, CLEAR);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, SET);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_A);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_OR_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_A_OR_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_OR_NOT_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_A_OR_NOT_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_AND_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_A_AND_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_AND_NOT_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, NOT_A_AND_NOT_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_EQUALS_B);
    SO_ENGINE_DEFINE_ENUM_VALUE(Operation, A_NOT_EQUALS_B);

    SO_ENGINE_SET_MF_ENUM_TYPE(operation, Operation);
    isBuiltIn = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoBoolOperation::~SoBoolOperation()
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
SoBoolOperation::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    int	na = a.getNum();
    int nb = b.getNum();
    int noperation = operation.getNum();
    int nout = max(na,nb,noperation);
    SO_ENGINE_OUTPUT(output, SoMFBool, setNum(nout));
    SO_ENGINE_OUTPUT(inverse, SoMFBool, setNum(nout));

    for (int i=0; i<nout; i++) {
	SbBool va = a[clamp(i,na)];
	SbBool vb = b[clamp(i,nb)];
	SbBool result;
	switch (operation[clamp(i,noperation)]) {
	case CLEAR:		result = FALSE;			break;
	case SET:		result = TRUE;			break;
	case A:			result = va;			break;
	case NOT_A:		result = !va;			break;
	case B:			result = vb;			break;
	case NOT_B:		result = !vb;			break;
	case A_OR_B:		result = va || vb;		break;
	case NOT_A_OR_B:	result = (!va) || vb;		break;
	case A_OR_NOT_B:	result = va || (!vb);		break;
	case NOT_A_OR_NOT_B:	result = (!va) || (!vb);	break;
	case A_AND_B:		result = va && vb;		break;
	case NOT_A_AND_B:	result = (!va) && (vb);		break;
	case A_AND_NOT_B:	result = va && (!vb);		break;
	case NOT_A_AND_NOT_B:	result = (!va) && (!vb);	break;
	case A_EQUALS_B:	result = (va == vb);		break;
	case A_NOT_EQUALS_B:	result = (va != vb);		break;
	}

	SO_ENGINE_OUTPUT(output, SoMFBool, set1Value(i, result));
	SO_ENGINE_OUTPUT(inverse, SoMFBool, set1Value(i, !result));
    }
}
