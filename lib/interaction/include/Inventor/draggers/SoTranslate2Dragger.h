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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the two dimensional translate dragger class.
 |	This is a simple dragger that allows an object to
 |	be translated within a plane. (defined by the plane normal (default
 |      normal is (0,0,1) and by the point initially hit by the user).
 |
 |      If the user presses the SHIFT key while dragging, the motion will
 |      constrain to linear motion along one of the two main axes of motion.
 |      The axis selected will be the primary axis followed by the user's
 |      dragging gesture.
 |
 |	The amount moved in the plane determines the translation.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoTranslate2Dragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoTranslate2Dragger.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |  The following parts in this dragger are created at construction time.
 |  'ResourceName' corresponds to the name of the default geometry for the
 |  part. The dragger's constructor gets the scene graph for 'ResourceName'
 |  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
 |
 |  Resource Name:                           Part Name:
 |
 |  translate2Translator                   - translator
 |  translate2TranslatorActive             - translatorActive
 |  translate2Feedback                     - feedback
 |  translate2FeedbackActive               - feedbackActive
 |
 |  NOTE: These are only shown if dragger 
 |        is moving and the axis is a 
 |        permissable direction of motion.
 |        When pressing <shift>, motion is
 |        constrained, and only 1 is displayed.
 |  translate2XAxisFeedback                - xAxisFeedback
 |  translate2YAxisFeedback                - yAxisFeedback
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TRANSLATE_2_DRAGGER_
#define  _SO_TRANSLATE_2_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/sensors/SoSensor.h>

class SbDict;
class SbPlaneProjector;
class SoFieldSensor;

// C-api: prefix=SoXlate2Drag
// C-api: public=translation
class SoTranslate2Dragger : public SoDragger
{
    SO_KIT_HEADER(SoTranslate2Dragger);

    SO_KIT_CATALOG_ENTRY_HEADER(translatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator);
    SO_KIT_CATALOG_ENTRY_HEADER(translatorActive);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(feedback);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackActive);
    SO_KIT_CATALOG_ENTRY_HEADER(axisFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(xAxisFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(yAxisFeedback);

  public:
    // Constructors
    SoTranslate2Dragger();

    SoSFVec3f translation;

  SoINTERNAL public:
    static void initClass();  // initialize the class

  protected:

    SbVec3f         worldRestartPt; // used if SHIFT key goes down to 
				    // initiate a new gesture.
    SbPlaneProjector *planeProj; // projector for planar scaling

    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void finishCB( void *, SoDragger * );

    SoFieldSensor *fieldSensor;
    static void fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    void dragStart();
    void drag();
    void dragFinish();

    // Callback for pressing and releasing the meta keys
    static void	metaKeyChangeCB( void *, SoDragger *);

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual ~SoTranslate2Dragger();

  private:
    int     translateDir;   // Used for 1-D translation,
    SbBool  shftDown;  // used to keep track of shift modifier key

    static const char geomBuffer[];
};    

#endif  /* _SO_TRANSLATE_2_DRAGGER_ */



