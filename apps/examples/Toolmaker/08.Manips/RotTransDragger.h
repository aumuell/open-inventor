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

/*------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 8, example 14.
 *
 *  Header file for "RotTransDragger"
 *
 *  This is the source file for the RotTransDragger.
 *  It is a compound dragger which allows independent rotation 
 *  in the x,y,and z directions as well as translation along a 
 *  line. It is an example of how to write a compound dragger.
 *
 *  It is composed of a TranslateRadialDragger and 
 *  3 SoRotateCylindricalDraggers bound into one dragger.
 *
 *  Clicking on the TranslateRadialDragger piece results in a
 *  translational motion along the line formed by the center of
 *  the dragger and the point on the dragger that was hit.
 *
 *  Clicking a cylinder rotator results in a rotation about 
 *  either the x,y,or z axis.
 *
 *  Geometry resources and part names for this dragger:
 *
 *  Resource Names:                     Part Names:
 *rotTransTranslatorTranslator       translator.translator
 *rotTransTranslatorTranslatorActive translator.translatorActive
 *rotTransTranslatorFeedback         translator.feedback
 *rotTransTranslatorFeedbackActive   translator.feedbackActive
 *
 *rotTransRotatorRotator             XRotator.rotator
 *rotTransRotatorRotatorActive       XRotator.rotatorActive
 *rotTransRotatorFeedback            XRotator.feedback
 *rotTransRotatorFeedbackActive      XRotator.feedbackActive
 *     (and similarly for parts of the YRotator and ZRotator)
 *
 *-----------------------------------------------------------*/
 
#ifndef  _ROT_TRANS_DRAGGER_
#define  _ROT_TRANS_DRAGGER_

#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>

class TranslateRadialDragger;
class SoRotateCylindricalDragger;

class RotTransDragger : public SoDragger
{
   SO_KIT_HEADER(RotTransDragger);

   // Makes the dragger surround other objects
   SO_KIT_CATALOG_ENTRY_HEADER(surroundScale);
   // Keeps the dragger evenly sized in all 3 dimensions
   SO_KIT_CATALOG_ENTRY_HEADER(antiSquish);

   // The translating dragger...
   SO_KIT_CATALOG_ENTRY_HEADER(translator);

   // The X and Z rotators need to be turned so as to orient 
   // correctly. So create a separator part and put an 
   // SoRotation node and the dragger underneath.
   SO_KIT_CATALOG_ENTRY_HEADER(XRotatorSep);
   SO_KIT_CATALOG_ENTRY_HEADER(XRotatorRot);
   SO_KIT_CATALOG_ENTRY_HEADER(XRotator);

   SO_KIT_CATALOG_ENTRY_HEADER(YRotator);

   SO_KIT_CATALOG_ENTRY_HEADER(ZRotatorSep);
   SO_KIT_CATALOG_ENTRY_HEADER(ZRotatorRot);
   SO_KIT_CATALOG_ENTRY_HEADER(ZRotator);

  public:
  
   // Constructor
   RotTransDragger();

   // These fields reflect state of the dragger at all times.
   SoSFRotation rotation;
   SoSFVec3f   translation;

   // This should be called once after SoInteraction::init().
   static void initClass();

  protected:

   // These sensors insures that the motionMatrix is updated 
   // when the fields are changed from outside.
   SoFieldSensor *rotFieldSensor;
   SoFieldSensor *translFieldSensor;
   static void fieldSensorCB(void *, SoSensor *);

   // This function is invoked by the child draggers when they 
   // change their value.
   static void valueChangedCB(void *, SoDragger *);

   // Called at the beginning and end of each dragging motion.
   // Tells the "surroundScale" part to recalculate.
   static void invalidateSurroundScaleCB(void *, SoDragger *);

   // This will detach/attach the fieldSensor.
   // It is called at the end of the constructor (to attach).
   // and at the start/end of SoBaseKit::readInstance()
   // and on the new copy at the start/end of SoBaseKit::copy()
   // Returns the state of the node when this was called.
   virtual SbBool setUpConnections( SbBool onOff, 
				SbBool doItAlways = FALSE);

   // This allows us to specify that certain parts do not
   // write out. We'll use this on the antiSquish and
   // surroundScale parts.
   virtual void setDefaultOnNonWritingFields();

  private:

   static const char geomBuffer[];

   // Destructor.
   ~RotTransDragger();
};   

#endif  /* _ROT_TRANS_DRAGGER_ */
