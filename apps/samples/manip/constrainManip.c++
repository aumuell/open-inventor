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

/*----------------------------------------------------------------
 *
 *  Read in a scene. Search for draggers and manipulators,
 *  adding constraints to them when found.
 *
 *  Display the scene in a viewer.
 *----------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>

#include <Inventor/actions/SoSearchAction.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

#include <Inventor/draggers/SoDragger.h>
#include <Inventor/draggers/SoCenterballDragger.h>
#include <Inventor/manips/SoTransformManip.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/nodekits/SoSceneKit.h>

typedef struct MyCBStruct {
    SbVec3f translateMin;
    SbVec3f translateMax;
    SbVec3f scaleMin;
    SbVec3f scaleMax;
    float   rotateMin;
    float   rotateMax;
    float   prevAngle;
    SbVec3f axis;
    SbRotation startRotation;
    SoTransformManip *manip;
};

#define CLAMP(v,min,max) v = (v < min) ? min : ((v > max) ? max : v)

// Limits translation between translateMin and translateMax
void
limitTranslation( SoSFVec3f *translation, MyCBStruct *stuff )
{
    SbVec3f xl = translation->getValue();
    CLAMP( xl[0], stuff->translateMin[0], stuff->translateMax[0] );
    CLAMP( xl[1], stuff->translateMin[1], stuff->translateMax[1] );
    CLAMP( xl[2], stuff->translateMin[2], stuff->translateMax[2] );
    if ( xl != translation->getValue() )
	(*translation) = xl;
}

// Limits center between translateMin and translateMax
void
limitCenter( SoSFVec3f *center, MyCBStruct *stuff )
{
    SbVec3f xl = center->getValue();
    CLAMP( xl[0], stuff->translateMin[0], stuff->translateMax[0] );
    CLAMP( xl[1], stuff->translateMin[1], stuff->translateMax[1] );
    CLAMP( xl[2], stuff->translateMin[2], stuff->translateMax[2] );
    if ( xl != center->getValue() )
	(*center) = xl;
}

// Limits scale between scaleMin and scaleMax
// Also doesn't  let scale go below 0.0
void
limitScale( SoSFVec3f *scaleFactor, MyCBStruct *stuff )
{
    SbVec3f scl = scaleFactor->getValue();
    CLAMP( scl[0], stuff->scaleMin[0], stuff->scaleMax[0] );
    CLAMP( scl[1], stuff->scaleMin[1], stuff->scaleMax[1] );
    CLAMP( scl[2], stuff->scaleMin[2], stuff->scaleMax[2] );
    CLAMP( scl[0], 0, stuff->scaleMax[0] );
    CLAMP( scl[1], 0, stuff->scaleMax[1] );
    CLAMP( scl[2], 0, stuff->scaleMax[2] );
    if ( scl != scaleFactor->getValue() )
	(*scaleFactor) = scl;
}

// Constrains the rotation to go around the initial direction of rotation
// until the dragging is complete.
// Limits amount of rotation from starting point to within range
// rotateMin < theta < rotateMax

void
limitRotate( SoSFRotation *rotation, MyCBStruct *stuff )
{
    // First, see if we need to determine the axis of rotation.
    // We need to do this the first time the dragger rotates, when axis==(0,0,0)
    if ( stuff->axis == SbVec3f(0,0,0) ) {
	SbRotation increment 
	    = stuff->startRotation.inverse() * rotation->getValue();
	increment.getValue( stuff->axis, stuff->prevAngle );
	if (stuff->prevAngle == 0.0) {
	    stuff->axis.setValue(0,0,0);
	    return;
	}
    }

    // Now we have an axis of rotation.
    // Constrain whatever the rotation is to be a rotation about this axis.
    SbBool madeAChange = FALSE;
    SbRotation rotSinceStart 
	= stuff->startRotation.inverse() * rotation->getValue();
    SbRotation newRotSinceStart;

    // First, append a rotation that will undo any wobble of our axis.
    SbVec3f wobbledAxis;
    rotSinceStart.multVec( stuff->axis, wobbledAxis );
    SbRotation undoWobble( wobbledAxis, stuff->axis );

    float   testAngle;
    SbVec3f testAxis;
    undoWobble.getValue( testAxis, testAngle );
    if (testAngle > .001) {
	newRotSinceStart = rotSinceStart * undoWobble;
	madeAChange = TRUE;
    }
    else
	newRotSinceStart = rotSinceStart;

    // Next, see what the angle of rotation has been.
    newRotSinceStart.getValue( testAxis, testAngle );
    if ( testAxis.dot( stuff->axis ) < 0.0 )
	testAngle *= -1.0;
    // Make this angle as close as possible to prevAngle
    while ( stuff->prevAngle - testAngle > 2 * M_PI )
	testAngle += M_PI;
    while ( testAngle - stuff->prevAngle > 2 * M_PI )
	testAngle -= M_PI;
    float clampAngle = testAngle;
    CLAMP( clampAngle, stuff->rotateMin, stuff->rotateMax );
    if ( clampAngle != testAngle ) {
        newRotSinceStart.setValue( stuff->axis, clampAngle );
	madeAChange = TRUE;
    }
    stuff->prevAngle = clampAngle;

    if ( madeAChange ) {
	SbRotation newFinalRot = stuff->startRotation * newRotSinceStart;
	(*rotation) = newFinalRot;
    }
}

// Dragger Start CB
// If there's a rotation field, records the starting rotation.
// Clears out the axis.
void
dragStartCB( void *stuff, SoDragger *dragger )
{
    MyCBStruct *myStuff = (MyCBStruct *) stuff;
    myStuff->axis.setValue(0,0,0);
    myStuff->prevAngle = 0.0;
    SoField *rotField = dragger->getField( "rotation" );
    if ( rotField != NULL )
	myStuff->startRotation = ((SoSFRotation *) rotField)->getValue();
    else 
	myStuff->startRotation = SbRotation::identity();
}

// Manip Value Changed CB
// Tells the dragger to limit each of the parent manip's fields: 
// translation, scaleFactor, and rotation.
void
manipValueChangedCB( void *stuff, SoDragger *)
{
    MyCBStruct *myStuff = (MyCBStruct *) stuff;
    SoTransformManip *m = myStuff->manip;

    if ( !m->isOfType( SoCenterballManip::getClassTypeId() ))
	limitTranslation( &(m->translation), myStuff ); 
    else
	limitCenter( &(m->center), myStuff ); 
    limitScale( &(m->scaleFactor), myStuff ); 
    limitRotate( &(m->rotation), myStuff ); 
}

// Dragger Value Changed CB
// Limits each of the fields: translation, scaleFactor, and rotation
// if they are present.
void
valueChangedCB( void *stuff, SoDragger *dragger )
{
    MyCBStruct *myStuff = (MyCBStruct *) stuff;
    SoField    *myField;

    if ( !dragger->isOfType( SoCenterballDragger::getClassTypeId() )) {
        myField = dragger->getField( "translation" );
	if ( myField != NULL ) 
	    limitTranslation( (SoSFVec3f *)myField, myStuff ); 
    }
    else {
	myField = dragger->getField( "center" );
	if ( myField != NULL ) 
	    limitCenter( (SoSFVec3f *)myField, myStuff ); 
    }

    myField = dragger->getField( "scaleFactor" );
    if ( myField != NULL ) 
	limitScale( (SoSFVec3f *)myField, myStuff ); 

    myField = dragger->getField( "rotation" );
    if ( myField != NULL ) 
	limitRotate( (SoSFRotation *)myField, myStuff ); 
}

void
main(int argc, char **argv)
{
   int curArg = 0;

   SbBool doConstraints = TRUE;
   float translateMin = -3.0;
   float translateMax =  3.0;
   float scaleMin = 0.0001;
   float scaleMax = 3.0;
   float rotateMin = -2.0;
   float rotateMax = 2.0;

   char *fileName = NULL;

   // Try to read the command line...
   curArg = 1;
   while (curArg < argc ) {
      if ( !strcmp(argv[curArg], "-noConstraints")) {
         curArg++;
	 doConstraints = FALSE;
      }
      else if ( !strcmp(argv[curArg], "-translateMin")) {
         curArg++;
         sscanf( argv[curArg++], "%f", &translateMin );
      }
      else if ( !strcmp(argv[curArg], "-translateMax")) {
         curArg++;
         sscanf( argv[curArg++], "%f", &translateMax );
      }
      else if ( !strcmp(argv[curArg], "-scaleMin")) {
         curArg++;
         sscanf( argv[curArg++], "%f", &scaleMin );
      }
      else if ( !strcmp(argv[curArg], "-scaleMax")) {
         curArg++;
         sscanf( argv[curArg++], "%f", &scaleMax );
      }
      else {
	 // get the filename
	 fileName = argv[curArg++];
      }
   }

    
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);


   SoSeparator *root = new SoSeparator;
   root->ref();

   SoInput myInput;
   if (fileName == NULL)
       fileName = "simpleDraggers.iv";
   if ( !myInput.openFile( fileName ) ) {
      fprintf(stderr, "ERROR - could not open file %s\n", fileName );
      fprintf(stderr, "usage-- constrainManip [-noConstraints] ] [-translateMin x] [-translateMax x] [-scaleMin x] [-scaleMax x] [-rotateMin] [-rotateMax] fileName\n");
      fprintf(stderr, "        where translateMin, translateMax, scaleMin, scaleMax, rotateMin, and rotateMax are constraints to be put on the draggers and manips.\n");
      exit(1);
   }

   SoSeparator *fileContents = SoDB::readAll( &myInput );
   root->addChild(fileContents);

   // Search for draggers and add constraints.
   SoSearchAction sa;
   SoPathList pathList;
   int i;

  // Set up values for callback.
  MyCBStruct *myCBStruct = new MyCBStruct;
  myCBStruct->translateMin.setValue( translateMin, translateMin, translateMin);
  myCBStruct->translateMax.setValue( translateMax, translateMax, translateMax);
  myCBStruct->scaleMin.setValue( scaleMin, scaleMin, scaleMin);
  myCBStruct->scaleMax.setValue( scaleMax, scaleMax, scaleMax);
  myCBStruct->rotateMin = rotateMin;
  myCBStruct->rotateMax = rotateMax;

    if (doConstraints) {
  // Add callbacks to all draggers that constrain motion:
      sa.setType( SoDragger::getClassTypeId() );
      sa.setInterest( SoSearchAction::ALL );
      sa.apply( root );
      pathList = sa.getPaths();
      for ( i = 0; i < pathList.getLength(); i++ ) {
         SoDragger *d = (SoDragger *) (pathList[i])->getTail();
         d->addStartCallback(&dragStartCB, myCBStruct);
         d->addValueChangedCallback(&valueChangedCB, myCBStruct);
      }

  // Add callbacks to all transform manips that constrain motion:
      sa.setType( SoTransformManip::getClassTypeId() );
      sa.setInterest( SoSearchAction::ALL );
      sa.apply( root );
      pathList = sa.getPaths();
      for ( i = 0; i < pathList.getLength(); i++ ) {
         SoTransformManip *m = (SoTransformManip *) (pathList[i])->getTail();
	  MyCBStruct *cbs = new MyCBStruct;
	  cbs->translateMin.setValue( translateMin, translateMin, translateMin);
	  cbs->translateMax.setValue( translateMax, translateMax, translateMax);
	  cbs->scaleMin.setValue( scaleMin, scaleMin, scaleMin);
	  cbs->scaleMax.setValue( scaleMax, scaleMax, scaleMax);
	  cbs->rotateMin = rotateMin;
	  cbs->rotateMax = rotateMax;
	  cbs->manip = m;
         SoDragger *d = m->getDragger();
         d->addStartCallback(&dragStartCB, cbs);
         d->addValueChangedCallback(&manipValueChangedCB, cbs);
      }
  }

   SoXtExaminerViewer *myVwr = new SoXtExaminerViewer(myWindow);
   myVwr->setSceneGraph(root);
   myVwr->setTitle("Manip Test");
   myVwr->viewAll();
   myVwr->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
