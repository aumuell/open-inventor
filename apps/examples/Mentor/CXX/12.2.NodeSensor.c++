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
 *  This is an example from the Inventor Mentor,
 *  chapter 12, example 2.
 *
 *  Using getTriggerNode/getTriggerField methods of the data
 *  sensor.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/sensors/SoNodeSensor.h>

// Sensor callback function:
static void
rootChangedCB(void *, SoSensor *s)
{
   // We know the sensor is really a data sensor:
   SoDataSensor *mySensor = (SoDataSensor *)s;
    
   SoNode *changedNode = mySensor->getTriggerNode();
   SoField *changedField = mySensor->getTriggerField();
    
   printf("The node named '%s' changed\n",
            changedNode->getName().getString());

   if (changedField != NULL) {
      SbName fieldName;
      changedNode->getFieldName(changedField, fieldName);
      printf(" (field %s)\n", fieldName.getString());
   } else {
      printf(" (no fields changed)\n");
   }
}

int
main(int , char **)
{
   SoDB::init();

   SoSeparator *root = new SoSeparator;
   root->ref();
   root->setName("Root");

   SoCube *myCube = new SoCube;
   root->addChild(myCube);
   myCube->setName("MyCube");

   SoSphere *mySphere = new SoSphere;
   root->addChild(mySphere);
   mySphere->setName("MySphere");

   SoNodeSensor *mySensor = new SoNodeSensor;

   mySensor->setPriority(0);
   mySensor->setFunction(rootChangedCB);
   mySensor->attach(root);

   // Now, make a few changes to the scene graph; the sensor's
   // callback function will be called immediately after each
   // change.
   myCube->width = 1.0;
   myCube->height = 2.0;
   mySphere->radius = 3.0;
   root->removeChild(mySphere);
}
