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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 7, examples 4 through 8.
 *
 *  Source file for "JumpingJackKit" 
 *------------------------------------------------------------*/

#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodekits/SoShapeKit.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>

#include "JumpingJackKit.h"


SO_KIT_SOURCE(JumpingJackKit);

void
JumpingJackKit::initClass()
{
   SO_KIT_INIT_CLASS(JumpingJackKit, 
      SoBaseKit, "BaseKit");
}

JumpingJackKit::JumpingJackKit()
{
   SO_KIT_CONSTRUCTOR(JumpingJackKit);

   // Add the body parts to the catalog...
   SO_KIT_ADD_CATALOG_ENTRY(body, SoShapeKit, 
      TRUE, this,, TRUE);
   SO_KIT_ADD_CATALOG_ENTRY(head, SoShapeKit, 
      TRUE, this,, TRUE);
   SO_KIT_ADD_CATALOG_ENTRY(leftArm, SoShapeKit, 
      TRUE, this,, TRUE);
   SO_KIT_ADD_CATALOG_ENTRY(rightArm, SoShapeKit, 
      TRUE, this,, TRUE);
   SO_KIT_ADD_CATALOG_ENTRY(leftLeg, SoShapeKit, 
      TRUE, this,, TRUE);
   SO_KIT_ADD_CATALOG_ENTRY(rightLeg, SoShapeKit, 
      TRUE, this,, TRUE);

   SO_KIT_INIT_INSTANCE();

   createInitialJack();
}

JumpingJackKit::~JumpingJackKit()
{
}

// This kit is made up entirely of SoShapeKits.
// Since SoShapeKits do not affect state, neither does this.
SbBool
JumpingJackKit::affectsState() const
{
   return FALSE;
}

// Set up parts for default configuration of the jumping jack
void
JumpingJackKit::createInitialJack()
{
   // Create the head.
   SoSphere *headSphere = new SoSphere;
   setPart("head.shape", headSphere);

   // Create the body.
   SoCube *bodyCube = new SoCube;
   setPart("body.shape", bodyCube);

   // Create the limbs
   SoCylinder *limbCylinder = new SoCylinder;
   setPart("leftLeg.shape",  limbCylinder);
   setPart("leftArm.shape",  limbCylinder);
   setPart("rightLeg.shape", limbCylinder);
   setPart("rightArm.shape", limbCylinder);

   // Place the body and head
   set("body.transform", "scaleFactor 1 2 1");
   set("head.transform", "translation 0 3 0");

   // Place the limbs
   set("leftArm.transform",  "scaleFactor 0.5 1.5 0.5");
   set("leftLeg.transform",  "scaleFactor 0.5 1.5 0.5");
   set("rightArm.transform", "scaleFactor 0.5 1.5 0.5");
   set("rightLeg.transform", "scaleFactor 0.5 1.5 0.5");
   set("leftArm.transform",  "center 0 1 0");
   set("leftLeg.transform",  "center 0 1 0");
   set("rightArm.transform", "center 0 1 0");
   set("rightLeg.transform", "center 0 1 0");
   set("leftArm.transform",  "translation -1  1   0.5");
   set("leftLeg.transform",  "translation -1 -2.5 0.5");
   set("rightArm.transform", "translation  1  1   0.5");
   set("rightLeg.transform", "translation  1 -2.5 0.5");

   // Create the Event Callback to make jack jump.
   // When it receives a mouse button event, it will
   // call the method jumpJackJump.
   SoEventCallback *myEventCB = new SoEventCallback;
   myEventCB->addEventCallback(
      SoMouseButtonEvent::getClassTypeId(), 
      JumpingJackKit::jumpJackJump, this);
   setPart("callbackList[0]",  myEventCB);
}

// Animates the jumping jack (called by the "eventCallback[0]" 
// part when a left mouse button press occurs).
void
JumpingJackKit::jumpJackJump(void *userData, 
                             SoEventCallback *myEventCB)
{
   const SoEvent *myEvent = myEventCB->getEvent();

   // see if it's a left mouse down event
   if (SO_MOUSE_PRESS_EVENT(myEvent, BUTTON1)) {
      JumpingJackKit *myJack = (JumpingJackKit *) userData;

      // See if the jumping jack was picked.
      const SoPickedPoint *myPickedPoint;
      myPickedPoint = myEventCB->getPickedPoint();
      if (myPickedPoint && myPickedPoint->getPath() &&
         myPickedPoint->getPath()->containsNode(myJack)) {

         // The jumping jack was picked. Make it jump!
         SoTransform *myXf;
         SbVec3f zAxis(0,0,1);
         SbRotation noRot = SbRotation::identity();

         myXf = SO_GET_PART(myJack,
	    "leftArm.transform",SoTransform);
         if (myXf->rotation.getValue() == noRot)
            myXf->rotation.setValue(zAxis ,-1.6);
         else
            myXf->rotation.setValue(noRot);

         myXf = SO_GET_PART(myJack,
	    "leftLeg.transform",SoTransform);
         if (myXf->rotation.getValue() == noRot)
            myXf->rotation.setValue(zAxis ,-1.2);
         else
            myXf->rotation.setValue(noRot);
         myXf = SO_GET_PART(myJack,
	    "rightArm.transform",SoTransform);
         if (myXf->rotation.getValue() == noRot)
            myXf->rotation.setValue(zAxis , 1.6);
         else
            myXf->rotation.setValue(noRot);

         myXf = SO_GET_PART(myJack,
	    "rightLeg.transform",SoTransform);
         if (myXf->rotation.getValue() == noRot)
            myXf->rotation.setValue(zAxis , 1.2);
         else
            myXf->rotation.setValue(noRot);
         myEventCB->setHandled();
      }
   }
}

