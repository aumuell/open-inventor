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

/*-------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 8, example 18 
 *
 *  Header file for "Coordinate3Manip"
 *
 *  This is a subclass of SoCoordinate3.
 *  It adds functionality to add a dragger as a hidden child. 
 *  The child, when dragged, will affect the fields of this 
 *  SoCoordinate3Manip through a collection of methods added 
 *  in this subclass.
 *
 *----------------------------------------------------------*/


#ifndef  _COORDINATE3_MANIP_
#define  _COORDINATE3_MANIP_

#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/sensors/SoFieldSensor.h>

class Coordinate3Manip : public SoCoordinate3
{
   SO_NODE_HEADER(Coordinate3Manip);

  public:

   // Constructor
   Coordinate3Manip();

   // The index of the 'point' field that will be edited 
   // by our child-dragger.
   SoSFInt32 editIndex;

   // Returns the dragger node being employed by this manip.
   SoDragger *getDragger();

   virtual SoNode *copy(SbBool copyConnections = FALSE) const;

   // For replacing a regular SoCoordinate3 node with this
   // manipulator.
   SbBool replaceNode(SoPath *p);

   // For replacing this manipulator with a regular 
   // SoCoordinate3 node.
   SbBool replaceManip(SoPath *p, SoCoordinate3 *newOne) const;

   // These functions implement all actions for this manip.
   // They first traverse the children, then use the 
   // SoCoordinate3 version of the actions. They traverse first 
   // so that the SoCoordinate3 will affect objects which 
   // follow it in the tree, but not the dragger-child.
   virtual void doAction(SoAction *action);
   virtual void callback(SoCallbackAction *action);
   virtual void GLRender(SoGLRenderAction *action);
   virtual void getBoundingBox(SoGetBoundingBoxAction *action);
   virtual void getMatrix(SoGetMatrixAction *action);
   virtual void handleEvent(SoHandleEventAction *action);
   virtual void pick(SoPickAction *action);
   virtual void search(SoSearchAction *action);

   // call this after SoInteraction::init();
   static void initClass();

   virtual SoChildList *getChildren() const;

  protected:

   // When the dragger moves, this interprets the translation 
   // field of the dragger and sets the point field of this 
   // node accordingly.
   static void valueChangedCB(void *,SoDragger *);

   // When the point field of this node changes, moves the
   // child-dragger to a new location, if necessary.
   SoFieldSensor *pointFieldSensor;
   static void fieldSensorCB(void *, SoSensor *);

   // Establishes the given dragger as the new child-dragger
   void setDragger(SoDragger *newDragger);

   // The hidden children.
   SoChildList *children;

  private:

   // Destructor
   ~Coordinate3Manip();

   int getNumChildren() const { return (children->getLength()); }
};    

#endif  /* _COORDINATE3_MANIP_ */
