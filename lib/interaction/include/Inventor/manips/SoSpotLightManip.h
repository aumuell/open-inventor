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

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This file defines a manipulator for a spot light nodes.
//      It is subclassed from SoSpotLight.
//
////////////////////////////////////////////////////////////////////////

 
#ifndef  _SO_SPOT_LIGHT_MANIP_
#define  _SO_SPOT_LIGHT_MANIP_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/sensors/SoFieldSensor.h>

// C-api: prefix=SoSpotLtManip
class SoSpotLightManip : public SoSpotLight
{
    SO_NODE_HEADER(SoSpotLightManip);

  public:
    // Constructors
    SoSpotLightManip();

    // C-api: name=getDrag
    SoDragger *getDragger();

    SbBool replaceNode( SoPath *p );
    SbBool replaceManip(SoPath *p, SoSpotLight *newOne ) const;

  SoEXTENDER public:

    // These functions implement all actions for nodekits.
    // They first traverse the children, then use the SoSpotLight
    // version of the actions. They traverse first so that the light
    // will affect objects which follow it in the tree, but not the
    // dragger-child.
    virtual void doAction( SoAction *action );
    virtual void callback( SoCallbackAction *action );
    virtual void GLRender( SoGLRenderAction *action );
    virtual void getBoundingBox( SoGetBoundingBoxAction *action );
    virtual void getMatrix(SoGetMatrixAction *action );
    virtual void handleEvent( SoHandleEventAction *action );
    virtual void pick( SoPickAction *action );
    virtual void search( SoSearchAction *action );

  SoINTERNAL public:

    static void initClass();

    virtual SoChildList *getChildren() const;

  protected:

    // Redefines this to also copy the dragger
    virtual void	copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

    static void transferFieldValues( const SoSpotLight *from, 
				    SoSpotLight *to);

    SoFieldSensor *locationFieldSensor;
    SoFieldSensor *directionFieldSensor;
    SoFieldSensor *angleFieldSensor;
    SoFieldSensor *colorFieldSensor;
    static void fieldSensorCB(void *, SoSensor *);
    static void valueChangedCB(void *,SoDragger *);

    void setDragger( SoDragger *newDragger );

    SoChildList *children;

    virtual ~SoSpotLightManip();

  private:
    int getNumChildren() const { return (children->getLength()); }
};    

#endif  /* _SO_SPOT_LIGHT_MANIP_ */
