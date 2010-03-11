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
* Copyright (C) 1990-93   Silicon Graphics, Inc.
*
_______________________________________________________________________
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
|
|   $Revision: 1.1.1.1 $
|
|   Classes:
|      Subclasses of multiple sliders that act on specific fields of 
|      specific types of nodes.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#include <math.h>
#include <X11/StringDefs.h>

#include <Inventor/sensors/SoSensor.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <SoXtMultiSliders.h>
#include <SoXtSliderTool.h>

#define TOL    0.0001
#define PI     3.141592653589793
#define DEGREES_TO_RADIANS     PI / 180.0
#define RADIANS_TO_DEGREES     180.0 / PI

static void extract_xyz( SbMatrix inMat, float &x, float &y, float &z )
{
    y = asin( inMat[2][0]);
    if( fabs(inMat[0][0])<TOL && fabs(inMat[1][0])<TOL){
	/*  cos( y ) near 0  */
	x = atan2(inMat[0][1], -1 * inMat[0][2]);
	z = 0.;
    }
    else{
	x = atan2( -1 * inMat[2][1],inMat[2][2]);
	z = atan2( -1 * inMat[1][0],inMat[0][0]);
    }
}

static void rot_xyz( float x, float y, float z, SbMatrix &out_mat )
{
    float      f_sin[3], f_cos[3];
    f_sin[0] = sin( x );  f_sin[1] = sin( y );
    f_sin[2] = sin( z );
    f_cos[0] = cos( x ); f_cos[1] = cos( y );
    f_cos[2] = cos( z );

    out_mat[0][0] =  f_cos[2] * f_cos[1];
    out_mat[1][0] = -f_sin[2] * f_cos[1];
    out_mat[2][0] =           f_sin[1];
    out_mat[3][0] = 0.0;

    out_mat[0][1] =  f_cos[2] * f_sin[1] * f_sin[0]  + f_sin[2] * f_cos[0];
    out_mat[1][1] = -f_sin[2] * f_sin[1] * f_sin[0]  + f_cos[2] * f_cos[0];
    out_mat[2][1] = -f_cos[1] * f_sin[0];
    out_mat[3][1] = 0.0;

    out_mat[0][2] = -f_cos[2] * f_sin[1] * f_cos[0] + f_sin[2] * f_sin[0];
    out_mat[1][2] =  f_sin[2] * f_sin[1] * f_cos[0] + f_cos[2] * f_sin[0];
    out_mat[2][2] =  f_cos[1] * f_cos[0];
    out_mat[3][2] = 0.0;

    out_mat[0][3] = 0.0;
    out_mat[1][3] = 0.0;
    out_mat[2][3] = 0.0;
    out_mat[3][3] = 1.0;
}

/*   Find a set of rotations equivalent to the 'new' ones           */
/*   that are closest in value to the 'old' ones                    */
/*  (Useful in adjusting rotations extracted from matrices.)        */
/*                                                                  */
/*  Assume that order of rotations is x then y then z. In this case,*/
/*  rot( r0 ) x      --> rot( r1 ) y      --> rot( r2 ) z           */
/*  is the same as:                                                 */
/*  rot( PI + r0 ) x --> rot( PI - r1 ) y --> rot( PI + r2 ) z      */
/*                                                                  */
static void match_xyz_rots( float oldx, float oldy, float oldz,
			    float &newx, float &newy, float &newz )
{

    int i;
    float R1[3],R2[3], Rold[3];
    float sum1, sum2;

    Rold[0] = oldx;
    Rold[1] = oldy;
    Rold[2] = oldz;

    /*  R1 and R2 are equivalent rots (see above)  */
    R1[0] = newx; R1[1] = newy; R1[2] = newz;
    R2[0] = PI + newx; R2[1] = PI - newy; R2[2] = PI + newz;
    /*  map rots into interval nearest 'last frame' rots  */
    for( i=0; i<3; i++){
	while( R1[i] <  Rold[i] - PI ) 
	       R1[i] += 2. * PI;
	while( R2[i] <  Rold[i] - PI ) 
	       R2[i] += 2. * PI;
	while( R1[i] >= Rold[i] + PI ) 
	       R1[i] -= 2. * PI;
	while( R2[i] >= Rold[i] + PI ) 
	       R2[i] -= 2. * PI;
    }
    /*  see whether R1 or R2 better matches 'last frame' rots  */
    sum1 = fabs(Rold[0]-R1[0]) + fabs(Rold[1]-R1[1]) + fabs(Rold[2]-R1[2]);
    sum2 = fabs(Rold[0]-R2[0]) + fabs(Rold[1]-R2[1]) + fabs(Rold[2]-R2[2]);
    if(sum1<sum2) {
	newx = R1[0]; 
	newy = R1[1]; 
	newz = R1[2]; 
    }
    else { 
	newx = R2[0]; 
	newy = R2[1]; 
	newz = R2[2]; 
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtTransMultiSlider
//
///////////////////////////////////////////////////////////////////////////////

SoXtTransMultiSlider::SoXtTransMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_TRANSLATE_SLIDER_MINIMUM,
				SO_TRANSLATE_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_TRANSLATE_SLIDER_MINIMUM,
				SO_TRANSLATE_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_TRANSLATE_SLIDER_MINIMUM,
				SO_TRANSLATE_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtTransMultiSlider::~SoXtTransMultiSlider() {}

SbBool
SoXtTransMultiSlider::validNodeType(SoNode *node)
{
    return ((node->isOfType(SoTransform::getClassTypeId())) ||
    	    (node->isOfType(SoTranslation::getClassTypeId())));
}

void
SoXtTransMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbVec3f      val;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	val = ((SoTransform *)_editNode)->translation.getValue();

	if ( val[0] != xX || val[1] != xY || val[2] != xZ ) { // COPY TO NODE
	    ((SoTransform *)_editNode)->translation.setValue( xX, xY, xZ );
	}
    }
}

void
SoXtTransMultiSlider::importValuesFromInventor()
{
    SbVec3f      val;

    if ( _editNode ) {

	// get values from inventor
	val = ((SoTransform *)_editNode)->translation.getValue();

	// set values if necessary
	if ( val[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( val[0] );
	if ( val[1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( val[1] );
	if ( val[2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( val[2] );
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtScaleMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtScaleMultiSlider::SoXtScaleMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool( 
				form, NULL, TRUE, 
				SO_SCALE_SLIDER_MINIMUM,
				SO_SCALE_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool( 
				form, NULL, TRUE, 
				SO_SCALE_SLIDER_MINIMUM,
				SO_SCALE_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool( 
				form, NULL, TRUE, 
				SO_SCALE_SLIDER_MINIMUM,
				SO_SCALE_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtScaleMultiSlider::~SoXtScaleMultiSlider() {}

SbBool
SoXtScaleMultiSlider::validNodeType(SoNode *node)
{
    return ((node->isOfType(SoTransform::getClassTypeId())) ||
    	    (node->isOfType(SoScale::getClassTypeId())));
}

void
SoXtScaleMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbVec3f      val;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	val = ((SoTransform *)_editNode)->scaleFactor.getValue();

	if ( val[0] != xX || val[1] != xY || val[2] != xZ ) { // COPY INTO NODE
	    ((SoTransform *)_editNode)->scaleFactor.setValue( xX, xY, xZ );
	}
    }
}

void
SoXtScaleMultiSlider::importValuesFromInventor()
{
    SbVec3f      val;

    if ( _editNode ) {

	// get values from inventor
	val = ((SoTransform *)_editNode)->scaleFactor.getValue();

	// set values if necessary
	if ( val[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( val[0] );
	if ( val[1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( val[1] );
	if ( val[2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( val[2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtRotateMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtRotateMultiSlider::SoXtRotateMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtRotateMultiSlider::~SoXtRotateMultiSlider() {}

SbBool
SoXtRotateMultiSlider::validNodeType(SoNode *node)
{
    return ((node->isOfType(SoTransform::getClassTypeId())) ||
    	    (node->isOfType(SoRotation::getClassTypeId())));
}

void
SoXtRotateMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbMatrix     theMatrix;
    SbRotation   theRotation;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// convert from degrees to radians
	xX *= DEGREES_TO_RADIANS;
	xY *= DEGREES_TO_RADIANS;
	xZ *= DEGREES_TO_RADIANS;

	// set values in inventor
	rot_xyz( xX, xY, xZ, theMatrix );

	theRotation.setValue( theMatrix );
	((SoTransform *)_editNode)->rotation.setValue( theRotation );
    }
}

void
SoXtRotateMultiSlider::importValuesFromInventor()
{
    SbMatrix     theMatrix;
    SbRotation   theRotation;
    float  	 xX, xY, xZ, x, y, z;

    if ( _editNode ) {

	// get values from inventor
	theRotation = ((SoTransform *)_editNode)->rotation.getValue();
	theRotation.getValue( theMatrix );
        extract_xyz( theMatrix, x, y, z );

	// get values from sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// convert from degrees to radians
	xX *= DEGREES_TO_RADIANS;
	xY *= DEGREES_TO_RADIANS;
	xZ *= DEGREES_TO_RADIANS;

	// match inventor values as closely as possible to the ones being 
	// displayed
	match_xyz_rots( xX, xY, xZ, x, y, z );

	// convert from radians to degrees
	x *= RADIANS_TO_DEGREES;
	y *= RADIANS_TO_DEGREES;
	z *= RADIANS_TO_DEGREES;

	// set values if necessary
	if ( x != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( x );
	if ( y != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( y );
	if ( z != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( z );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtScaleOrientationMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtScaleOrientationMultiSlider::SoXtScaleOrientationMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_ROTATE_SLIDER_MINIMUM,
				SO_ROTATE_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtScaleOrientationMultiSlider::~SoXtScaleOrientationMultiSlider() {}

SbBool
SoXtScaleOrientationMultiSlider::validNodeType(SoNode *node)
{
    return ((node->isOfType(SoTransform::getClassTypeId())) ||
    	    (node->isOfType(SoRotation::getClassTypeId())));
}

void
SoXtScaleOrientationMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbMatrix     theMatrix;
    SbRotation   theRotation;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// convert from degrees to radians
	xX *= DEGREES_TO_RADIANS;
	xY *= DEGREES_TO_RADIANS;
	xZ *= DEGREES_TO_RADIANS;

	// set values in inventor
	rot_xyz( xX, xY, xZ, theMatrix );

	theRotation.setValue( theMatrix );
	((SoTransform *)_editNode)->scaleOrientation.setValue( theRotation );
    }
}

void
SoXtScaleOrientationMultiSlider::importValuesFromInventor()
{
    SbMatrix     theMatrix;
    SbRotation   theRotation;
    float  	 xX, xY, xZ, x, y, z;

    if ( _editNode ) {

	// get values from inventor
	theRotation = ((SoTransform *)_editNode)->scaleOrientation.getValue();
	theRotation.getValue( theMatrix );
        extract_xyz( theMatrix, x, y, z );

	// get values from sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// convert from degrees to radians
	xX *= DEGREES_TO_RADIANS;
	xY *= DEGREES_TO_RADIANS;
	xZ *= DEGREES_TO_RADIANS;

	// match inventor values as closely as possible to the ones being 
	// displayed
	match_xyz_rots( xX, xY, xZ, x, y, z );

	// convert from radians to degrees
	x *= RADIANS_TO_DEGREES;
	y *= RADIANS_TO_DEGREES;
	z *= RADIANS_TO_DEGREES;

	// set values if necessary
	if ( x != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( x );
	if ( y != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( y );
	if ( z != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( z );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtCenterMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtCenterMultiSlider::SoXtCenterMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(form);
    _subComponentArray[1] = new SoXtSliderTool(form);
    _subComponentArray[2] = new SoXtSliderTool(form);

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtCenterMultiSlider::~SoXtCenterMultiSlider() {}

SbBool
SoXtCenterMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoTransform::getClassTypeId()));
}

void
SoXtCenterMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbVec3f      val;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	val = ((SoTransform *)_editNode)->center.getValue();

	if ( val[0] != xX || val[1] != xY || val[2] != xZ ) { // COPY INTO NODE
	    ((SoTransform *)_editNode)->center.setValue( xX, xY, xZ );
	}
    }
}

void
SoXtCenterMultiSlider::importValuesFromInventor()
{
    SbVec3f      val;

    if ( _editNode ) {

	// get values from inventor
	val = ((SoTransform *)_editNode)->center.getValue();

	// set values if necessary
	if ( val[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( val[0] );
	if ( val[1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( val[1] );
	if ( val[2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( val[2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtAmbientColorMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtAmbientColorMultiSlider::SoXtAmbientColorMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtAmbientColorMultiSlider::~SoXtAmbientColorMultiSlider() {}

SbBool
SoXtAmbientColorMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtAmbientColorMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    const        SbVec3f *soColor;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->ambientColor.getValues( 0 );

	// copy into node if necessary
	if (soColor[0][0] != xX || soColor[0][1] != xY || soColor[0][2] != xZ) {
	    ((SoMaterial *)_editNode)->ambientColor.setValue( xX, xY, xZ );
	    ((SoMaterial *)_editNode)->ambientColor.setIgnored(FALSE);
	}
    }
}

void
SoXtAmbientColorMultiSlider::importValuesFromInventor()
{
    const        SbVec3f *soColor;

    if ( _editNode ) {

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->ambientColor.getValues( 0 );

	// set values if necessary
	if ( soColor[0][0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soColor[0][0] );
	if ( soColor[0][1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( soColor[0][1] );
	if ( soColor[0][2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( soColor[0][2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtDiffuseColorMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtDiffuseColorMultiSlider::SoXtDiffuseColorMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtDiffuseColorMultiSlider::~SoXtDiffuseColorMultiSlider() {}

SbBool
SoXtDiffuseColorMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtDiffuseColorMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    const        SbVec3f *soColor;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->diffuseColor.getValues( 0 );

	// copy into node if necessary
	if (soColor[0][0] != xX || soColor[0][1] != xY || soColor[0][2] != xZ) {
	    ((SoMaterial *)_editNode)->diffuseColor.setValue( xX, xY, xZ );
	    ((SoMaterial *)_editNode)->diffuseColor.setIgnored(FALSE);
	}
    }
}

void
SoXtDiffuseColorMultiSlider::importValuesFromInventor()
{
    const        SbVec3f *soColor;

    if ( _editNode ) {

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->diffuseColor.getValues( 0 );

	// set values if necessary
	if ( soColor[0][0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soColor[0][0] );
	if ( soColor[0][1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( soColor[0][1] );
	if ( soColor[0][2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( soColor[0][2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtSpecularColorMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtSpecularColorMultiSlider::SoXtSpecularColorMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtSpecularColorMultiSlider::~SoXtSpecularColorMultiSlider() {}

SbBool
SoXtSpecularColorMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtSpecularColorMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    const        SbVec3f *soColor;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->specularColor.getValues( 0 );

	if (soColor[0][0] != xX || soColor[0][1] != xY || soColor[0][2] != xZ) {
	    ((SoMaterial *)_editNode)->specularColor.setValue( xX, xY, xZ );
	    ((SoMaterial *)_editNode)->specularColor.setIgnored(FALSE);
	}
    }
}

void
SoXtSpecularColorMultiSlider::importValuesFromInventor()
{
    const        SbVec3f *soColor;

    if ( _editNode ) {

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->specularColor.getValues( 0 );

	// set values if necessary
	if ( soColor[0][0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soColor[0][0] );
	if ( soColor[0][1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( soColor[0][1] );
	if ( soColor[0][2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( soColor[0][2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtEmissiveColorMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtEmissiveColorMultiSlider::SoXtEmissiveColorMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtEmissiveColorMultiSlider::~SoXtEmissiveColorMultiSlider() {}

SbBool
SoXtEmissiveColorMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtEmissiveColorMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    const        SbVec3f *soColor;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->emissiveColor.getValues( 0 );

	if (soColor[0][0] != xX || soColor[0][1] != xY || soColor[0][2] != xZ) {
	    ((SoMaterial *)_editNode)->emissiveColor.setValue( xX, xY, xZ );
	    ((SoMaterial *)_editNode)->emissiveColor.setIgnored(FALSE);
	}
    }
}

void
SoXtEmissiveColorMultiSlider::importValuesFromInventor()
{
    const        SbVec3f *soColor;

    if ( _editNode ) {

	// get values from inventor
	soColor = ((SoMaterial *)_editNode)->emissiveColor.getValues( 0 );

	// set values if necessary
	if ( soColor[0][0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soColor[0][0] );
	if ( soColor[0][1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( soColor[0][1] );
	if ( soColor[0][2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( soColor[0][2] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtShininessMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtShininessMultiSlider::SoXtShininessMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 1;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtShininessMultiSlider::~SoXtShininessMultiSlider() {}

SbBool
SoXtShininessMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtShininessMultiSlider::exportValuesToInventor()
{
    float        xVal;
    const        float *soShininess;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xVal = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();

	// get values from inventor
	soShininess = ((SoMaterial *)_editNode)->shininess.getValues( 0 );

	if (soShininess[0] != xVal ) {
	    ((SoMaterial *)_editNode)->shininess.setValue( xVal );
	    ((SoMaterial *)_editNode)->shininess.setIgnored(FALSE);
	}
    }
}

void
SoXtShininessMultiSlider::importValuesFromInventor()
{
    const        float *soShininess;

    if ( _editNode ) {

	// get values from inventor
	soShininess = ((SoMaterial *)_editNode)->shininess.getValues( 0 );

	// set values if necessary
	if ( soShininess[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soShininess[0] );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtTransparencyMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtTransparencyMultiSlider::SoXtTransparencyMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 1;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtTransparencyMultiSlider::~SoXtTransparencyMultiSlider() {}

SbBool
SoXtTransparencyMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoMaterial::getClassTypeId()));
}

void
SoXtTransparencyMultiSlider::exportValuesToInventor()
{
    float        xVal;
    const        float *soTransparency;

    if ( _editNode ) {  

	// get values from x-windows sliders
	xVal = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();

	// get values from inventor
	soTransparency = ((SoMaterial *)_editNode)->transparency.getValues( 0 );

	if (soTransparency[0] != xVal ) {
	    ((SoMaterial *)_editNode)->transparency.setValue( xVal );
	    ((SoMaterial *)_editNode)->transparency.setIgnored(FALSE);
	}
    }
}

void
SoXtTransparencyMultiSlider::importValuesFromInventor()
{
    const        float *soTransparency;

    if ( _editNode ) {

	// get values from inventor
	soTransparency = ((SoMaterial *)_editNode)->transparency.getValues( 0 );

	// set values if necessary
	if ( soTransparency[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soTransparency[0] );
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtLightIntensityMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtLightIntensityMultiSlider::SoXtLightIntensityMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 1;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtLightIntensityMultiSlider::~SoXtLightIntensityMultiSlider() {}

SbBool
SoXtLightIntensityMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoLight::getClassTypeId()));
}

void
SoXtLightIntensityMultiSlider::exportValuesToInventor()
{
    float        xVal;
    float        soIntensity;

    if ( _editNode ) {

	// get values from x-windows sliders
	xVal = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();

	// get values from inventor
	soIntensity = ((SoLight *)_editNode)->intensity.getValue();

	if (soIntensity != xVal ) {
	    ((SoLight *)_editNode)->intensity.setValue( xVal );
	}
    }
}

void
SoXtLightIntensityMultiSlider::importValuesFromInventor()
{
    float        soIntensity;

    if ( _editNode ) {

	// get values from inventor
	soIntensity = ((SoLight *)_editNode)->intensity.getValue();

	// set values if necessary
	if ( soIntensity != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soIntensity );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtLightColorMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtLightColorMultiSlider::SoXtLightColorMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 3;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[1] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );
    _subComponentArray[2] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_UNIT_SLIDER_MINIMUM,
				SO_UNIT_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtLightColorMultiSlider::~SoXtLightColorMultiSlider() {}

SbBool
SoXtLightColorMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoLight::getClassTypeId()));
}

void
SoXtLightColorMultiSlider::exportValuesToInventor()
{
    float        xX, xY, xZ;
    SbColor      soColor;

    if ( _editNode ) {

	// get values from x-windows sliders
	xX = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue();
	xY = ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue();
	xZ = ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue();

	// get values from inventor
	soColor = ((SoLight *)_editNode)->color.getValue();

	// copy into node if necessary
	if (soColor[0] != xX || soColor[1] != xY || soColor[2] != xZ) {
	    ((SoLight *)_editNode)->color.setValue( xX, xY, xZ );
	}
    }
}

void
SoXtLightColorMultiSlider::importValuesFromInventor()
{
    SbColor      soColor;

    if ( _editNode ) {

	// get values from inventor
	soColor = ((SoLight *)_editNode)->color.getValue();

	// set values if necessary
	if ( soColor[0] != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soColor[0] );
	if ( soColor[1] != ((SoXtSliderTool *) _subComponentArray[1])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[1])->toolSetValue( soColor[1] );
	if ( soColor[2] != ((SoXtSliderTool *) _subComponentArray[2])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[2])->toolSetValue( soColor[2] );
    }
}
///////////////////////////////////////////////////////////////////////////////
//
//   Class: SoXtFovMultiSlider
//
///////////////////////////////////////////////////////////////////////////////
SoXtFovMultiSlider::SoXtFovMultiSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtMultiSlider(
	    parent,
	    name, 
	    buildInsideParent,
	    FALSE)  // tell parent not to build just yet
{
    _numSubComponents = 1;
    _subComponentArray = new ( SoXtSliderSetBase * [_numSubComponents] );

    Widget form = buildForm(getParentWidget());
    
    _subComponentArray[0] = new SoXtSliderTool(
				form, NULL, TRUE, 
				SO_FOV_SLIDER_MINIMUM,
				SO_FOV_SLIDER_MAXIMUM );

    for( int i = 0; i < _numSubComponents; i++ )
	((SoXtSliderTool *)_subComponentArray[i])->setMultiSlider( this );

    SoXtMultiSlider::buildWidget(form);
    setBaseWidget(form);
}

SoXtFovMultiSlider::~SoXtFovMultiSlider() {}

SbBool
SoXtFovMultiSlider::validNodeType(SoNode *node)
{
    return (node->isOfType(SoCamera::getClassTypeId()));
}

void
SoXtFovMultiSlider::exportValuesToInventor()
{
    float        xFov;
    float        soFov;

    if ( _editNode ) {

	// get value from x-windows sliders
	xFov = ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue() * DEGREES_TO_RADIANS;

	// get values from inventor
	if ( _editNode->isOfType(SoPerspectiveCamera::getClassTypeId()) )
	    soFov = ((SoPerspectiveCamera *)_editNode)->heightAngle.getValue();
	else
	    soFov = ((SoOrthographicCamera *)_editNode)->height.getValue();

	// copy into node if necessary
	if (soFov != xFov ) {
	    if (_editNode->isOfType(SoPerspectiveCamera::getClassTypeId()))
		((SoPerspectiveCamera *)_editNode)->heightAngle.setValue( xFov );
	    else
		((SoOrthographicCamera *)_editNode)->height.setValue( xFov );
	    
	}
    }
}

void
SoXtFovMultiSlider::importValuesFromInventor()
{
    float      soFov;

    if ( _editNode ) {

	// get values from inventor
	if (_editNode->isOfType(SoPerspectiveCamera::getClassTypeId()))
	    soFov = ((SoPerspectiveCamera *)_editNode)->heightAngle.getValue() * RADIANS_TO_DEGREES;
	else
	    soFov = ((SoOrthographicCamera *)_editNode)->height.getValue();

	// set values if necessary
	if (soFov != ((SoXtSliderTool *) _subComponentArray[0])->getSliderValue())
	    ((SoXtSliderTool *) _subComponentArray[0])->toolSetValue( soFov );
    }
}
