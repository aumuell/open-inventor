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
 *  chapter 10.
 *
 *  Header file for "SceneTumble" component.
 *------------------------------------------------------------*/

#ifndef _SCENE_TUMBLE_
#define _SCENE_TUMBLE_

#include <Inventor/Xt/SoXtRenderArea.h>

class SoSeparator;
class SoPerspectiveCamera;
class SoRotation;
class SoTimerSensor;

class SceneTumble : public SoXtRenderArea {

   public:
   
   // Constructor for public consumption
   SceneTumble(
      Widget parent = NULL,
      const char *name = NULL, 
      SbBool buildInsideParent = TRUE);
   ~SceneTumble();

   virtual void	setSceneGraph(SoNode *newScene);
   virtual SoNode *getSceneGraph();

   void setTumbling(SbBool onOff);
   SbBool isTumbling() const;
    
    
   protected:
   
   // Constructor subclasses can call if they don't want the
   // widget built right away (i.e. the subclass wants to create
   // a container widget first.)
   SceneTumble(
      Widget parent,
      const char *name, 
      SbBool buildInsideParent, 
      SbBool buildNow);

   Widget buildWidget(Widget parent);

   void doTumbleAnimation();

   void setSpeed(int s) { speed = s; }
   int getSpeed() const { return speed; }

   Widget speedSlider;
    
    
   private:
   
   void constructorCommon(SbBool buildNow);
   static void visibilityChangeCB(void *userData, SbBool visible);
   static void animationSensorCB(void *userData, SoSensor *);
   static void speedCB(Widget, XtPointer, XtPointer);
   
   SoNode      *userScene;
   SoSeparator *root;
   SoPerspectiveCamera *camera;
   SoRotation *rotx;
   SoRotation *roty;
   SoRotation *rotz;
   int speed;
   SoTimerSensor *animationSensor;
}; 

#endif /* _SCENE_TUMBLE_ */
