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


#include <X11/Intrinsic.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSeparator.h>

#include "ButtonBoxEvent.h"
#include "DialEvent.h"
#include "DialNButton.h"

int dialVals[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// Map which button (1-32) into complexity (0-1).
void
buttonBoxCB(void *userData, SoEventCallback *cb)
{
   const ButtonBoxEvent *ev = 
      (const ButtonBoxEvent *) cb->getEvent();
	
   if (ev->getState() == SoButtonEvent::DOWN) {
      int which = ev->getButton();
      SoComplexity *complexity = (SoComplexity *) userData;
      complexity->value = (which - 1)/31.0;
   }
}

// Convert a dial value into the range [0.0,1.0]
float
getDialFloat(int d)
{
   float v;
   
   // Clamp the dial value to [-128,128]
   // so the conversion is possible.
   if (d <= -128)
      v = 0.0;
   else if (d >= 128)
      v = 1.0;
   else
      v = (d + 128)/256.0;
   
   return v;
}

// Convert a dial value into the range [-1.0,1.0]
float
getDialFloat2(int d)
{
   float v;
   
   // Clamp the dial value to [-128,128]
   // so the conversion is possible.
   if (d <= -128)
      v = -1.0;
   else if (d >= 128)
      v = 1.0;
   else
      v = d/128.0;
   
   return v;
}

// Use dials to change the material of the sphere
void
dialCB(void *userData, SoEventCallback *cb)
{
   const DialEvent *ev = 
      (const DialEvent *) cb->getEvent();
   
   SoDirectionalLight *light = (SoDirectionalLight *) userData;
   
   // Use dials to control a directional light:
   //   dial 0: red         dial 1: x component of direction
   //   dial 2: green       dial 3: y component of direction
   //   dial 4: blue        dial 5: z component of direction
   //   dial 6: intensity   dial 7: unused
   float x, y, z, r, g, b, f;
   switch (ev->getDial()) {
      case 0:
      case 2:
      case 4:
         light->color.getValue().getValue(r, g, b);
	 f = getDialFloat(ev->getValue());

	 if (ev->getDial() == 0)
	      r = f;
	 else if (ev->getDial() == 2)
	      g = f;
	 else b = f;
	 
	 light->color.setValue(r, g, b);
         break;

      case 6:
	 light->intensity.setValue(getDialFloat(ev->getValue()));
         break;
	 
      case 1:
      case 3:
      case 5:
         light->direction.getValue().getValue(x, y, z);
	 f = getDialFloat2(ev->getValue());
	 
	 if (ev->getDial() == 1)
	      x = f;
	 else if (ev->getDial() == 3)
	      y = f;
	 else z = f;
	 
	 light->direction.setValue(x, y, z);
         break;
	 
      default: 
	 break;
   }
}   

SoNode *
buildSceneGraph()
{
   SoSeparator *sep = new SoSeparator;
   SoEventCallback *cb = new SoEventCallback;
   SoDirectionalLight *light = new SoDirectionalLight;
   SoComplexity *complexity = new SoComplexity;
   SoSphere *sphere = new SoSphere;
   
   sep->addChild(cb);
   sep->addChild(light);
   sep->addChild(complexity);
   sep->addChild(sphere);
   
   // Set starting values. For direction, set the z value
   // close to 0 so turning the dial does not cause jumpiness.
   light->color.setValue(.5, .5, .5);
   light->intensity.setValue(.5);
   light->direction.setValue(0, 0, -.01);
   
   complexity->value.setValue(1.0);
   
   // Set up event callbacks
   cb->addEventCallback(
      ButtonBoxEvent::getClassTypeId(),
      buttonBoxCB, 
      complexity);
   cb->addEventCallback(
      DialEvent::getClassTypeId(),
      dialCB, 
      light);
   
   return sep;
}

void
main(int , char *argv[])
{
   Widget mainWindow = SoXt::init(argv[0]);
   
   if (! DialNButton::exists()) {
       fprintf(stderr, "Sorry, no dial and button box on this display!\n");
       return;
   }
   
   ButtonBoxEvent::initClass();
   DialEvent::initClass();
   
   SoXtExaminerViewer *vwr = new SoXtExaminerViewer(mainWindow);
   vwr->setSceneGraph(buildSceneGraph());
   vwr->setTitle("Dial And Button Device");
   vwr->setViewing(FALSE);   // come up in pick mode
   vwr->setHeadlight(FALSE); // we supply our own light

   DialNButton *db = new DialNButton;
   vwr->registerDevice(db);
   
   vwr->show();
   SoXt::show(mainWindow);
   SoXt::mainLoop();
}
