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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Abstract base camera node class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CAMERA_
#define  _SO_CAMERA_

#include <Inventor/SbBox.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFEnum.h>

class SbViewportRegion;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCamera
//
//  Abstract base class Camera node.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoCam
// C-api: public= viewportMapping, position, orientation, aspectRatio
// C-api: public= nearDistance, farDistance, focalDistance
class SoCamera : public SoNode {

    SO_NODE_ABSTRACT_HEADER(SoCamera);

  public:
    enum ViewportMapping {	// What will happen if the camera and
				// viewport aspect ratios differ?
	
				       // The first 3 adjust the viewport
				       // to fit the camera.
        CROP_VIEWPORT_FILL_FRAME = 0,  // Draw filled frame around vp.
        CROP_VIEWPORT_LINE_FRAME = 1,  // Draw frame in lines.
        CROP_VIEWPORT_NO_FRAME   = 2,  // Draw no frame.

	ADJUST_CAMERA            = 3,  // Adjust camera to fit viewport.
	LEAVE_ALONE              = 4   // Do nothing. Camera image may
				       // become stretched out of proportion
    };

    // NOTE: These fields are here so that all camera subclasses do
    // not have to define them separately. However, all subclasses
    // DO have to set up field data with these fields in them to make
    // things work properly.

    // Fields
    SoSFEnum		viewportMapping;// Treatment when aspectRatio not
					// same as viewport's aspectRatio
    SoSFVec3f		position;	// Location of viewpoint
    SoSFRotation	orientation;	// Orientation (rotation with
					// respect to (0,0,-1) vector)
    SoSFFloat		aspectRatio;	// Ratio of width to height of
					// view plane
    SoSFFloat		nearDistance;	// Distance from viewpoint to
					// view plane
    SoSFFloat		farDistance;	// Distance from viewpoint to
					// far clipping plane
    SoSFFloat	    	focalDistance;	// Distance from viewpoint to
					// point of focus.

    // Sets the orientation of the camera so that it points toward the
    // given target point while keeping the "up" direction of the
    // camera parallel to the positive y-axis. If this is not
    // possible, it uses the positive z-axis as "up".
    // C-api: name=ptAt
    void		pointAt(const SbVec3f &targetPoint);

    // Scales the height of the camera. This is a virtual function.
    // Perspective cameras will scale their 'heightAngle' field here, and ortho
    // cameras will scale their 'height' field.
    // C-api: expose
    virtual void	scaleHeight(float scaleFactor) = 0;

    // Fills in a view volume structure, based on the camera. If the
    // useAspectRatio field is not 0.0 (the default), the camera uses
    // that ratio instead of the one it has.
    // C-api: expose
    // C-api: name=getViewVol
    virtual SbViewVolume getViewVolume(float useAspectRatio = 0.0) const = 0;

    // Sets the camera up to view the scene under the given node or
    // defined by the given path. The near and far clipping planes
    // will be positioned 'slack' bounding sphere radii away from the
    // bounding box's center. A value of 1.0 will make the clipping
    // planes the tightest around the bounding sphere.
    void		viewAll(SoNode *sceneRoot,
				const SbViewportRegion &vpRegion,
				float slack = 1.0);
    // C-api: name=viewAllPath
    void		viewAll(SoPath *path,
				const SbViewportRegion &vpRegion,
				float slack = 1.0);

    // Returns the viewport region this camera would use to render
    // into a given viewport region, accounting for cropping
    // C-api: name=getVpBounds
    SbViewportRegion	getViewportBounds(const SbViewportRegion &region) const;

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	handleEvent(SoHandleEventAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    SoCamera();				// Makes this abstract
    virtual ~SoCamera();

    // Subclasses must define this method, which changes the position
    // and clipping planes of a camera to view the passed bounding box
    // using the given aspect ratio, without changing the height
    // scaling.
    virtual void	viewBoundingBox(const SbBox3f &box,
					float aspect, float slack) = 0;

    // Returns amount to jitter camera in normalized device
    // coordinates (after projection) for anti-aliasing during
    // multi-pass rendering
    virtual void	jitter(int numPasses, int curPass,
			       const SbViewportRegion &vpReg,
			       SbVec3f &jitterAmount) const;

  private:
    // Computes the view volume the camera represents, given a viewport region
    void		computeView(const SbViewportRegion &vpReg,
				    SbViewVolume &viewVol,
				    SbBool &changeRegion);

    // Sets relevant info in elements in state
    void		setElements(SoAction *action,
				    SbViewVolume &viewVol,
				    SbBool setRegion,
				    const SbViewportRegion &vpReg,
				    SbBool doJitter,
				    const SbVec3f &jitterAmount);

    // Draws cropping frame when rendering
    void		drawFrame(SoGLRenderAction *action,
				  const SbViewportRegion &vpReg,
				  const SbViewportRegion &croppedReg);

    // Returns a 2D sample point within a pixel
    static void		getJitterSample(int numPasses, int curPass,
					SbVec2f &samplePoint);
};

//////////////////////////////////////////////////////////////////////////////
//
// Commonly used camera aspect ratios
//

#define SO_ASPECT_SQUARE	1.00	     /* Square (1.000)		     */
#define SO_ASPECT_VIDEO		1.333333333  /* Video (0.75)		     */
#define SO_ASPECT_35mm_ACADEMY	1.371	     /* 35mm, Academy ap (.72939460) */
#define SO_ASPECT_16mm		1.369	     /* 16mm cinema (.730460189)     */
#define SO_ASPECT_35mm_FULL	1.33333	     /* 35mm cinema, full ap (0.75)  */
#define SO_ASPECT_70mm		2.287 	     /* 70 mm unsqueezed (.43725404) */
#define SO_ASPECT_CINEMASCOPE	2.35	     /* Cinemascope (.425531914)     */
#define SO_ASPECT_HDTV		1.777777777  /* HDTV (16:9) 		     */
#define SO_ASPECT_PANAVISION	2.361	     /* Panavision (.423549343)	     */
#define SO_ASPECT_35mm		1.5	     /* 35mm still camera (.666666)  */
#define SO_ASPECT_VISTAVISION	2.301	     /* Vistavision (.434593654)     */

#endif /* _SO_CAMERA_ */
