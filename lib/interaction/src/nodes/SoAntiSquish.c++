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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      SoAntiSquish
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/nodes/SoAntiSquish.h>

SO_NODE_SOURCE(SoAntiSquish);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoAntiSquish::SoAntiSquish()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoAntiSquish);
    isBuiltIn = TRUE;

    SO_NODE_ADD_FIELD(sizing,     (AVERAGE_DIMENSION) );
    SO_NODE_ADD_FIELD(recalcAlways,         (TRUE) );

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, X);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, Y);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, Z);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, AVERAGE_DIMENSION);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, BIGGEST_DIMENSION);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, SMALLEST_DIMENSION);
    SO_NODE_DEFINE_ENUM_VALUE(Sizing, LONGEST_DIAGONAL);

    SO_NODE_SET_SF_ENUM_TYPE(sizing, Sizing);

    savedAnswer        = SbMatrix::identity();
    savedInverseAnswer = SbMatrix::identity();

    recalcNextTime = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoAntiSquish::~SoAntiSquish()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculates an unsquished matrix based on the 'sizing' field and the
//    given matrix.
//
// Use: private
SbMatrix
SoAntiSquish::getUnsquishingMatrix( SbMatrix squishedMatrix,
				    SbBool /* doInverse */,
				    SbMatrix &inverseAnswer )
//
////////////////////////////////////////////////////////////////////////
{
    // Think of the squishedMatrix as the following chain of matrices, given the
    // results of SbMatrix::factor()
    // [T]  = translation 
    // [R]  = rotation 
    // [S]  = scale
    // [SO] = scaleOrientation
    // squishedMatrix = [SO-Inv][S][SO][R][T]

    SbVec3f  scaleV, translV;
    SbMatrix scaleOrientM, rotM, projM; 
    if (!squishedMatrix.factor(scaleOrientM,scaleV,rotM,translV,projM)) {
	// If the matrix was singular, then we can not unsquish it.
	// Return identity.
	SbMatrix answer = SbMatrix::identity();
	// Bug 323082: no longer a check for the doInverse flag. We
	// always set this
	inverseAnswer = SbMatrix::identity();
	savedAnswer = answer;
	savedInverseAnswer = inverseAnswer;
	return answer;
    }

#define TINY .00001
    for (int i = 0; i < 2; i++ ) {
	if (scaleV[i] < .00001)
	    scaleV[i] = TINY;
    }
#undef TINY

    // We want to append our squishedMatrix with a new matrix:
    // desiredM = [NEWSCALE][R][T]
    // where NEWSCALE is a uniform scale based on [S] and the sizing field
    float scl;

    Sizing whichSizing = (Sizing ) sizing.getValue();
    if ( whichSizing == X )
	scl = scaleV[0];
    else if ( whichSizing == Y )
	scl = scaleV[1];
    else if ( whichSizing == Z )
	scl = scaleV[2];
    else if ( whichSizing == AVERAGE_DIMENSION )
	scl = (scaleV[0] + scaleV[1] + scaleV[2]) / 3.0 ;
    else if ( whichSizing == BIGGEST_DIMENSION ) {
	scl =   (scaleV[0] >= scaleV[1] && scaleV[0] >= scaleV[2] ) ? scaleV[0]
	      : (scaleV[1] >= scaleV[2] )                           ? scaleV[1] 
	      :  scaleV[2];
    }
    else if ( whichSizing == SMALLEST_DIMENSION ) {
	scl =   (scaleV[0] <= scaleV[1] && scaleV[0] <= scaleV[2] ) ? scaleV[0]
	      : (scaleV[1] <= scaleV[2] )                           ? scaleV[1] 
	      :  scaleV[2];
    }
    else if ( whichSizing == LONGEST_DIAGONAL ) {
	// Determine the aggregate scaleOrientation-scale matrix
	SbMatrix aggregate;
	aggregate.setScale(scaleV);
	aggregate.multLeft( scaleOrientM );
	// Multiply each of 4 Diagonals (symmetry says we don't need all 8)
	// by this matrix.
	SbVec3f v1(1,1,1), v2(-1,1,1), v3(-1,-1,1), v4(1,-1,1);
	aggregate.multVecMatrix(v1,v1);
	aggregate.multVecMatrix(v2,v2);
	aggregate.multVecMatrix(v3,v3);
	aggregate.multVecMatrix(v4,v4);
	float ls[4];
	ls[0] = v1.length();
	ls[1] = v2.length();
	ls[2] = v3.length();
	ls[3] = v4.length();
	// Use the length of the biggest.
	scl = ls[0];
	for (int i = 0; i < 4; i++ )
	    if ( ls[i] > scl )
		scl = ls[i];
    }

    float invScl;
    invScl = 1.0 / scl;

    SbVec3f newScale( scl, scl, scl), newScaleInv( invScl, invScl, invScl );

    SbMatrix desiredM, tempM;

    desiredM.setTranslate(  translV );
    desiredM.multLeft(rotM);
    tempM.setScale( newScale );
    desiredM.multLeft(tempM);

    // Now find our answer, which does the following:
    // [answerM][squishMatrix] = [desiredM]
    // [answerM] = [desiredM][squishMatrixInverse]
    SbMatrix answerM = desiredM;
    answerM.multRight( squishedMatrix.inverse() );

    inverseAnswer = answerM.inverse();

    savedAnswer = answerM;
    savedInverseAnswer = inverseAnswer;

    return answerM;
}

void
SoAntiSquish::callback( SoCallbackAction *action )
{
    SoAntiSquish::doAction( action );
}

void
SoAntiSquish::GLRender( SoGLRenderAction *action )
{
    SoAntiSquish::doAction( action );
}

void
SoAntiSquish::getBoundingBox( SoGetBoundingBoxAction *action )
{
    //??? HACK!!!
    //    The recalc'ing action of these nodes behaves poorly because
    //    often, the first traversal after recalc() is called is
    //    during the getboundingboxaction applied by an SoSurroundScale node.
    //    This action is applied so the surround scale can figure out what
    //    to surround, and has poor state information for the antisquish.
    //    [1] it is not applied to a full path so it lacks the transforms
    //        from above.
    //    [2] the surround scale does not participate, so it lacks the scale
    //        usually issued by the surround scale.
    //
    //    To get around this, we do not undo our recalcNextTime flag in this 
    //    case.  We can identify the case by testing if we are below the 
    //    resetpath of the action.  If we are mistaken, it still can't hurt 
    //    to wait:
    //    since we're under the reset path it won't make a difference what
    //    we do here.
    if ( recalcAlways.getValue() == FALSE && recalcNextTime == TRUE ) {

	// return if we're below the reset path. I.E., if current traversal
	// path contains the reset path.
	const SoPath *cp = action->getCurPath();
	const SoPath *rp = action->getResetPath();
	if ( ! action->isResetBefore() && cp && rp && cp->containsPath(rp) )
	    return;
    }

    SoAntiSquish::doAction( action );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: protected

void
SoAntiSquish::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	&ctm = action->getMatrix();
    SbMatrix	&inv = action->getInverse();

    if ( recalcAlways.getValue() == TRUE || recalcNextTime == TRUE ) {
	SbMatrix answer, invAnswer;
	answer = getUnsquishingMatrix( ctm, TRUE, invAnswer );

	ctm.multLeft( answer );
	inv.multRight( invAnswer );
    }
    else {
	ctm.multLeft( savedAnswer );
	inv.multRight( savedInverseAnswer );
    }
}

void
SoAntiSquish::pick( SoPickAction *action )
{
    SoAntiSquish::doAction( action );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles any action derived from SoAction.
//
// Use: private

void
SoAntiSquish::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if ( recalcAlways.getValue() == TRUE || recalcNextTime == TRUE ) {

	SbMatrix oldCtm = SoModelMatrixElement::get(action->getState());

        SbMatrix answer, dummyM;
	answer = getUnsquishingMatrix( oldCtm, FALSE, dummyM );

	recalcNextTime = FALSE;
        SoModelMatrixElement::mult(action->getState(), this, answer );
    }
    else
        SoModelMatrixElement::mult(action->getState(), this, savedAnswer );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method sets a flag to recalculate our matrix next time an
//    action is applied.
//
// Use: protected
void
SoAntiSquish::recalc()
//
////////////////////////////////////////////////////////////////////////
{
    recalcNextTime = TRUE;
    touch();
}
