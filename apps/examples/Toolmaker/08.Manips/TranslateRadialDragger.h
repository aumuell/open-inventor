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
 *  chapter 8, example 2.
 *
 *  Header file for "TranslateRadialDragger"
 *
 *  This is a simple dragger which allows translation 
 *  along a line. It is used as an example of how to write a 
 *  simple dragger.
 *
 *  It operates by allowing the user to click on the dragger,
 *  then dragging along the line formed by the center of the 
 *  dragger and the point on the dragger that was hit.
 *
 *  Resource names and part names for this dragger are:
 *      Resource Name:                        Part Name:
 *	translateRadialTranslator             translator
 *	translateRadialTranslatorActive       translatorActive
 *	translateRadialFeedback               feedback
 *	translateRadialFeedbackActive         feedbackActive
 *
 *------------------------------------------------------------*/

#ifndef  _TRANSLATE_RADIAL_DRAGGER_
#define  _TRANSLATE_RADIAL_DRAGGER_

#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/sensors/SoFieldSensor.h>

class SbLineProjector;

class TranslateRadialDragger : public SoDragger
{
   SO_KIT_HEADER(TranslateRadialDragger);

   // Catalog entries for new parts added by this class...
   SO_KIT_CATALOG_ENTRY_HEADER(translatorSwitch);
   SO_KIT_CATALOG_ENTRY_HEADER(translator);
   SO_KIT_CATALOG_ENTRY_HEADER(translatorActive);
   SO_KIT_CATALOG_ENTRY_HEADER(feedbackRotate);
   SO_KIT_CATALOG_ENTRY_HEADER(feedbackSwitch);
   SO_KIT_CATALOG_ENTRY_HEADER(feedback);
   SO_KIT_CATALOG_ENTRY_HEADER(feedbackActive);

  public:
  
   // Constructor
   TranslateRadialDragger();

   // Field that will always contain the dragger's position
   SoSFVec3f translation;

   // Initialize the class. This should be called once
   // after SoInteraction::init().
   static void initClass();

  protected:

   void orientFeedbackGeometry(const SbVec3f &localDir);

   // projector used for calculating motion along a line.
   SbLineProjector *lineProj;

   // Static callback functions invoked by SoDragger when the 
   // mouse button goes down over this dragger, when the
   // mouse drags, and when the button is released.
   static void startCB(void *, SoDragger *);
   static void motionCB(void *, SoDragger *);
   static void finishCB(void *, SoDragger *);

   // These functions, invoked by the static callback 
   // functions, do all the work of moving the dragger.
   void dragStart();
   void drag();
   void dragFinish();

   // This sensor watches for changes to the translation field
   SoFieldSensor *fieldSensor;
   static void fieldSensorCB(void *, SoSensor *);

   // This callback updates the translation field when 
   // the dragger is moved.
   static void valueChangedCB(void *, SoDragger *);

   // This will detach/attach the fieldSensor.
   // It is called at the end of the constructor (to attach).
   // and at the start/end of SoBaseKit::readInstance()
   // and on the new copy at the start/end of SoBaseKit::copy()
   // Returns the state of the node when this was called.
   virtual SbBool setUpConnections( SbBool onOff, 
				SbBool doItAlways = FALSE);

  private:

   static const char geomBuffer[];

   // Destructor.
   ~TranslateRadialDragger();
};    

#endif  /* _TRANSLATE_RADIAL_DRAGGER_ */
