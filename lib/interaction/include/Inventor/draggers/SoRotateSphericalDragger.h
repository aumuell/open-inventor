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
 |	This file defines the RotateSpherical dragger class.
 |	This is a simple dragger that returns a rotation
 |	based on vectors from the center of the sphere to its surface.
 |
 |	The mouse position is projected onto a sphere determined
 |	by a center and the radius. The radius is determined from
 |	the distance from the center to the initial hit point.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoRotateSphericalDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoRotateSphericalDragger.
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
 |  rotateSphericalRotator                 - rotator
 |  rotateSphericalRotatorActive           - rotatorActive
 |  rotateSphericalFeedback                - feedback
 |  rotateSphericalFeedbackActive          - feedbackActive
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ROTATE_SPHERICAL_DRAGGER_
#define  _SO_ROTATE_SPHERICAL_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/sensors/SoSensor.h>


class SbDict;
class SbSphereProjector;
class SoFieldSensor;

// C-api: prefix=SoRotSphDrag
// C-api: public=rotation
class SoRotateSphericalDragger : public SoDragger
{
    SO_KIT_HEADER(SoRotateSphericalDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(rotatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator);
    SO_KIT_CATALOG_ENTRY_HEADER(rotatorActive);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(feedback);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackActive);

  public:
    // Constructors
    SoRotateSphericalDragger();

    SoSFRotation rotation;

    // Set/get a different sphere projector. The default uses an
    // SbSpherePlaneProjector. 
    // Pasing in NULL will cause the default type of projector to be used.
    // The projector will be deleted by this dragger when this dragger
    // is deleted.
    // Note that the center and radius of the sphere used by the
    // projector are determined by the dragger during dragStart.
    // C-api: name=setProj
    void			setProjector(SbSphereProjector *p);
    // C-api: name=getProj
    const SbSphereProjector *	getProjector() const	 { return sphereProj; }

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    SbMatrix        prevMotionMatrix; // used during interaction
    SbVec3f         prevWorldHitPt; // used during interaction
    SbSphereProjector	*sphereProj;	// Sphere projector to use.
    SbBool		userProj;	// using a user defined projector?

    // Redefines this to also copy the projector.
    // The projector needs to be copied since the user can set it.
    virtual void	copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void doneCB( void *, SoDragger * );

    SoFieldSensor *fieldSensor;
    static void fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    void	dragStart();
    void	drag();
    void	dragFinish();

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual ~SoRotateSphericalDragger();

  private:
    // Default geometry
    static const char geomBuffer[];
};    


#endif  /* _SO_ROTATE_SPHERICAL_DRAGGER_ */


