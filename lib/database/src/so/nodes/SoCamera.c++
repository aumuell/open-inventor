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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoCamera
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoFocalDistanceElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoGLProjectionMatrixElement.h>
#include <Inventor/elements/SoGLRenderPassElement.h>
#include <Inventor/elements/SoGLUpdateAreaElement.h>
#include <Inventor/elements/SoGLViewingMatrixElement.h>
#include <Inventor/elements/SoGLViewportRegionElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/nodes/SoCamera.h>
#include <stdlib.h>

SO_NODE_ABSTRACT_SOURCE(SoCamera);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoCamera::SoCamera()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoCamera);

    SO_NODE_ADD_FIELD(viewportMapping,	(ADJUST_CAMERA));
    SO_NODE_ADD_FIELD(position,		(0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(orientation,	(0.0, 0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(aspectRatio,	(SO_ASPECT_SQUARE));
    SO_NODE_ADD_FIELD(nearDistance,	(1.0));
    SO_NODE_ADD_FIELD(farDistance,	(10.0));
    SO_NODE_ADD_FIELD(focalDistance,  (SoFocalDistanceElement::getDefault()));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping, CROP_VIEWPORT_FILL_FRAME);
    SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping, CROP_VIEWPORT_LINE_FRAME);
    SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping, CROP_VIEWPORT_NO_FRAME);
    SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping, ADJUST_CAMERA);
    SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping, LEAVE_ALONE);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(viewportMapping, ViewportMapping);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoCamera::~SoCamera()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the orientation of the camera so that it points toward the
//    given target point while keeping the "up" direction of the
//    camera parallel to the positive y-axis. If this is not possible,
//    it uses the positive z-axis as "up".
//
// Use: public

void
SoCamera::pointAt(const SbVec3f &targetPoint)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	d(targetPoint - position.getValue());

    SbRotation	yRot(SbVec3f(0,1,0), atan2(-d[0], -d[2]));
    SbRotation  xRot(SbVec3f(1,0,0), atan2(d[1], sqrtf(d[0]*d[0] + d[2]*d[2])));

    orientation.setValue(xRot * yRot);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the camera up to view the scene under the given node. The
//    near and far clipping planes will be positioned 'slack' bounding
//    sphere radii away from the bounding box's center. A value of 1.0
//    will make the clipping planes the tightest around the bounding
//    sphere.
//
// Use: public

void
SoCamera::viewAll(SoNode *root, const SbViewportRegion &vpRegion, float slack)
//
////////////////////////////////////////////////////////////////////////
{
    SoGetBoundingBoxAction	action(vpRegion);

    // Crop the viewport region if necessary
    SbViewportRegion	croppedReg = getViewportBounds(vpRegion);

    // Find the bounding box of the scene, then adjust camera to view it
    action.apply(root);
    viewBoundingBox(action.getBoundingBox(),
		    croppedReg.getViewportAspectRatio(), slack);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the camera up to view the scene defined by the given path.
//    The near and far clipping planes will be positioned 'slack'
//    bounding sphere radii away from the bounding box's center. A
//    value of 1.0 will make the clipping planes the tightest around
//    the bounding sphere.
//
// Use: public

void
SoCamera::viewAll(SoPath *path, const SbViewportRegion &vpRegion, float slack)
//
////////////////////////////////////////////////////////////////////////
{
    SoGetBoundingBoxAction	action(vpRegion);

    // Crop the viewport region if necessary
    SbViewportRegion	croppedReg = getViewportBounds(vpRegion);

    // Find the bounding box of the scene, then adjust camera to view it
    action.apply(path);
    viewBoundingBox(action.getBoundingBox(),
		    croppedReg.getViewportAspectRatio(), slack);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the viewport region this camera would use to render into
//    a given viewport region, accounting for cropping.
//
// Use: public

SbViewportRegion
SoCamera::getViewportBounds(const SbViewportRegion &region) const
//
////////////////////////////////////////////////////////////////////////
{
    float	vpAspect, camAspect;

    // Start with the same region as we are given
    SbViewportRegion	croppedRegion = region;

    switch (viewportMapping.getValue()) {

      case CROP_VIEWPORT_FILL_FRAME:
      case CROP_VIEWPORT_LINE_FRAME:
      case CROP_VIEWPORT_NO_FRAME:

	vpAspect  = region.getViewportAspectRatio();
	camAspect = aspectRatio.getValue();

	// Make a smaller viewport that
	//     [a] fits within the old viewport and
	//     [b] uses the camera's aspect ratio

	if (camAspect > vpAspect)
	    croppedRegion.scaleHeight(vpAspect / camAspect);

	else if (camAspect < vpAspect)
	    croppedRegion.scaleWidth(camAspect / vpAspect);

	break;

      case ADJUST_CAMERA:
      case LEAVE_ALONE:
	// Neither of these adjusts the viewport size.
	break;
    }

    return croppedRegion;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs most actions that SoCamera supports.
//
// Use: extender

void
SoCamera::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume	viewVol;
    SbBool		changeRegion;

    // Get the current viewport region
    const SbViewportRegion &vpReg =
	SoViewportRegionElement::get(action->getState());

    // Compute the view volume
    SoCamera::computeView(vpReg, viewVol, changeRegion);

    // Set the state
    SbVec3f dummyJitter;
    setElements(action, viewVol, changeRegion,
		changeRegion ? getViewportBounds(vpReg) : vpReg,
		FALSE, dummyJitter);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the callback action.
//
// Use: extender

void
SoCamera::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCamera::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering on a camera node.
//
// Use: extender

void
SoCamera::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewportRegion	croppedReg;
    SbViewVolume	viewVol;
    SbMatrix		viewMat, projMat;
    SbVec2f		uaOrigin, uaSize;
    SbVec3f		jitterAmount;
    SbBool		changeRegion;
    SoState		*state = action->getState();

    // Get the current viewport region
    const SbViewportRegion &vpReg = SoGLViewportRegionElement::get(state);

    // Compute the view volume
    SoCamera::computeView(vpReg, viewVol, changeRegion);

    // Draw frame, if necessary, using current (full) viewport
    if (changeRegion) {
	croppedReg = getViewportBounds(vpReg);
	drawFrame(action, vpReg, croppedReg);
    }

    // Jitter the camera for anti-aliasing if doing multiple passes
    if (action->getNumPasses() > 1)
	jitter(action->getNumPasses(), SoGLRenderPassElement::get(state),
	       changeRegion ? croppedReg : vpReg, jitterAmount);

    // Set the state
    setElements(action, viewVol, changeRegion, croppedReg,
		action->getNumPasses() > 1, jitterAmount);

    // Compute and set culling volume if different from view volume
    if (! SoGLUpdateAreaElement::get(state, uaOrigin, uaSize)) {
	SbViewVolume cvv = viewVol.narrow(uaOrigin[0], uaOrigin[1],
					  uaSize[0],   uaSize[1]);
	SoModelMatrixElement::setCullMatrix(state, this, cvv.getMatrix());
    }
    // Otherwise, just set culling volume to be same as view volume
    else
	SoModelMatrixElement::setCullMatrix(state, this, viewVol.getMatrix());

    // Don't auto-cache above cameras:
    SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DONT_AUTO_CACHE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns amount to jitter camera in normalized device coordinates
//    (after projection) for anti-aliasing during multi-pass rendering.
//
// Use: protected, virtual

void
SoCamera::jitter(int numPasses, int curPass,
		 const SbViewportRegion &vpReg, SbVec3f &jitterAmount) const
//
////////////////////////////////////////////////////////////////////////
{
    // Get the current sample point within the pixel
    SbVec2f samplePoint;
    getJitterSample(numPasses, curPass, samplePoint);

    // Compute the jitter amount for the projection matrix. This
    // jitter will be in post-perspective space, which goes from -1 to
    // +1 in x and y. In this space, the size of a pixel is 2/width by
    // 2/height.
    const SbVec2s &vpSize = vpReg.getViewportSizePixels();
    jitterAmount[0] = samplePoint[0] * 2.0 / (float) vpSize[0];
    jitterAmount[1] = samplePoint[1] * 2.0 / (float) vpSize[1];
    jitterAmount[2] = 0.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a 2D sample point within a pixel.
//
// Use: private, static

void
SoCamera::getJitterSample(int numPasses, int curPass, SbVec2f &samplePoint)
//
////////////////////////////////////////////////////////////////////////
{
    // These arrays define filter kernels to be used when the total
    // number of passes is small. For N passes, use the smallest
    // kernel that is >= N.
    static const float	kernel2[2][2]   = {{ 0.246490,  0.249999 },
					   {-0.246490, -0.249999 }},
			kernel3[3][2]   = {{-0.373411, -0.250550 },
					   { 0.256263,  0.368119 },
					   { 0.117148, -0.117570 }},
			kernel4[4][2]   = {{-0.208147,  0.353730 },
					   { 0.203849, -0.353780 },
					   {-0.292626, -0.149945 },
					   { 0.296924,  0.149994 }},
			kernel8[8][2]   = {{-0.334818,  0.435331 },
					   { 0.286438, -0.393495 },
					   { 0.459462,  0.141540 },
					   {-0.414498, -0.192829 },
					   {-0.183790,  0.082102 },
					   {-0.079263, -0.317383 },
					   { 0.102254,  0.299133 },
					   { 0.164216, -0.054399 }},
			kernel15[15][2] = {{ 0.285561,  0.188437 },
					   { 0.360176, -0.065688 },
					   {-0.111751,  0.275019 },
					   {-0.055918, -0.215197 },
					   {-0.080231, -0.470965 },
					   { 0.138721,  0.409168 },
					   { 0.384120,  0.458500 },
					   {-0.454968,  0.134088 },
					   { 0.179271, -0.331196 },
					   {-0.307049, -0.364927 },
					   { 0.105354, -0.010099 },
					   {-0.154180,  0.021794 },
					   {-0.370135, -0.116425 },
					   { 0.451636, -0.300013 },
					   {-0.370610,  0.387504 }},
			kernel24[24][2] = {{ 0.030245,  0.136384 },
					   { 0.018865, -0.348867 },
					   {-0.350114, -0.472309 },
					   { 0.222181,  0.149524 },
					   {-0.393670, -0.266873 },
					   { 0.404568,  0.230436 },
					   { 0.098381,  0.465337 },
					   { 0.462671,  0.442116 },
					   { 0.400373, -0.212720 },
					   {-0.409988,  0.263345 },
					   {-0.115878, -0.001981 },
					   { 0.348425, -0.009237 },
					   {-0.464016,  0.066467 },
					   {-0.138674, -0.468006 },
					   { 0.144932, -0.022780 },
					   {-0.250195,  0.150161 },
					   {-0.181400, -0.264219 },
					   { 0.196097, -0.234139 },
					   {-0.311082, -0.078815 },
					   { 0.268379,  0.366778 },
					   {-0.040601,  0.327109 },
					   {-0.234392,  0.354659 },
					   {-0.003102, -0.154402 },
					   { 0.297997, -0.417965 }},
			kernel66[66][2] = {{ 0.266377, -0.218171 },
					   {-0.170919, -0.429368 },
					   { 0.047356, -0.387135 },
					   {-0.430063,  0.363413 },
					   {-0.221638, -0.313768 },
					   { 0.124758, -0.197109 },
					   {-0.400021,  0.482195 },
					   { 0.247882,  0.152010 },
					   {-0.286709, -0.470214 },
					   {-0.426790,  0.004977 },
					   {-0.361249, -0.104549 },
					   {-0.040643,  0.123453 },
					   {-0.189296,  0.438963 },
					   {-0.453521, -0.299889 },
					   { 0.408216, -0.457699 },
					   { 0.328973, -0.101914 },
					   {-0.055540, -0.477952 },
					   { 0.194421,  0.453510 },
					   { 0.404051,  0.224974 },
					   { 0.310136,  0.419700 },
					   {-0.021743,  0.403898 },
					   {-0.466210,  0.248839 },
					   { 0.341369,  0.081490 },
					   { 0.124156, -0.016859 },
					   {-0.461321, -0.176661 },
					   { 0.013210,  0.234401 },
					   { 0.174258, -0.311854 },
					   { 0.294061,  0.263364 },
					   {-0.114836,  0.328189 },
					   { 0.041206, -0.106205 },
					   { 0.079227,  0.345021 },
					   {-0.109319, -0.242380 },
					   { 0.425005, -0.332397 },
					   { 0.009146,  0.015098 },
					   {-0.339084, -0.355707 },
					   {-0.224596, -0.189548 },
					   { 0.083475,  0.117028 },
					   { 0.295962, -0.334699 },
					   { 0.452998,  0.025397 },
					   { 0.206511, -0.104668 },
					   { 0.447544, -0.096004 },
					   {-0.108006, -0.002471 },
					   {-0.380810,  0.130036 },
					   {-0.242440,  0.186934 },
					   {-0.200363,  0.070863 },
					   {-0.344844, -0.230814 },
					   { 0.408660,  0.345826 },
					   {-0.233016,  0.305203 },
					   { 0.158475, -0.430762 },
					   { 0.486972,  0.139163 },
					   {-0.301610,  0.009319 },
					   { 0.282245, -0.458671 },
					   { 0.482046,  0.443890 },
					   {-0.121527,  0.210223 },
					   {-0.477606, -0.424878 },
					   {-0.083941, -0.121440 },
					   {-0.345773,  0.253779 },
					   { 0.234646,  0.034549 },
					   { 0.394102, -0.210901 },
					   {-0.312571,  0.397656 },
					   { 0.200906,  0.333293 },
					   { 0.018703, -0.261792 },
					   {-0.209349, -0.065383 },
					   { 0.076248,  0.478538 },
					   {-0.073036, -0.355064 },
					   { 0.145087,  0.221726 }};

    // Determine which kernel to use and access the correct jitter amount
    if (numPasses == 2)
	samplePoint = kernel2[curPass];
    else if (numPasses == 3)
	samplePoint = kernel3[curPass];
    else if (numPasses == 4)
	samplePoint = kernel4[curPass];
    else if (numPasses <= 8)
	samplePoint = kernel8[curPass];
    else if (numPasses <= 15)
	samplePoint = kernel15[curPass];
    else if (numPasses <= 24)
	samplePoint = kernel24[curPass];
    else if (numPasses <= 66)
	samplePoint = kernel66[curPass];

    // If we don't have enough samples in the largest kernel, we'll
    // allocate an array and fill it with random samples to cover the
    // ones not in the kernel
    else {
	static float *	extraSamples = NULL;
	static int	numExtraSamples = 0;

	// Do the allocation (if necessary) only on the first pass, to
	// avoid unnecessary tests
	if (curPass == 1) {

	    int	numExtraNeeded = numPasses - 66;

	    if (numExtraSamples < numExtraNeeded) {
		if (extraSamples != NULL)
		    delete extraSamples;
		extraSamples = new float [numExtraNeeded * 2];

		// Set 2 coords of sample to random number between -1 and +1.
		for (int i = 0; i < 2 * numExtraNeeded; i++)
		    extraSamples[i] = 2.0 * drand48() - 1.0;
	    }
	}

	if (curPass < 66)
	    samplePoint = kernel66[curPass];
	else
	    samplePoint.setValue(extraSamples[(curPass - 66) * 2 + 0],
				 extraSamples[(curPass - 66) * 2 + 1]);

    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the getBoundingBox action. If the isCameraSpace() flag is
//    set in the action, this also transforms the model matrix to set
//    up camera space.
//
// Use: extender

void
SoCamera::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff first
    SoCamera::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up viewing info for event handling.
//
// Use: extender

void
SoCamera::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Set only a subset of the elements for this action

    SbViewVolume	viewVol;
    SbBool		changeRegion;
    SoState		*state = action->getState();

    // Get the current viewport region
    const SbViewportRegion &vpReg = SoViewportRegionElement::get(state);

    // Compute the view volume
    SoCamera::computeView(vpReg, viewVol, changeRegion);

    // Set necessary elements in state
    SoViewVolumeElement::set(state, this, viewVol);
    if (changeRegion)
	SoViewportRegionElement::set(state, getViewportBounds(vpReg));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up viewing info for ray picking.
//
// Use: extender

void
SoCamera::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCamera::doAction(action);

    // Tell the action how to set up world space view
    action->computeWorldSpaceRay();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes the view volume the camera represents, based on the
//    given viewport region. Returns TRUE in changeRegion if the
//    viewport region needs to be adjusted based on the current
//    viewport mapping, meaning that it has to be changed in the
//    state.
//
// Use: private

void
SoCamera::computeView(const SbViewportRegion &vpReg,
		      SbViewVolume &viewVol, SbBool &changeRegion)
//
////////////////////////////////////////////////////////////////////////
{
    float	camAspect, vpAspect;

    changeRegion = FALSE;

    camAspect = aspectRatio.getValue();

    switch (viewportMapping.getValue()) {
      case CROP_VIEWPORT_FILL_FRAME:
      case CROP_VIEWPORT_LINE_FRAME:
      case CROP_VIEWPORT_NO_FRAME:
	changeRegion = TRUE;
	viewVol = getViewVolume(camAspect);
	break;

      case ADJUST_CAMERA:
	vpAspect = vpReg.getViewportAspectRatio();
	viewVol = getViewVolume(vpAspect);
	// Adjust size of view volume if it will fill viewport better
 	if (vpAspect < 1.0)
 	    viewVol.scale(1.0 / vpAspect);
	break;

      case LEAVE_ALONE:
	// Do nothing. The window specified by the camera will be
	// stretched to fit into the current viewport.
	viewVol = getViewVolume(camAspect);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets relevant info in elements in state.
//
// Use: private

void
SoCamera::setElements(SoAction *action, SbViewVolume &viewVol,
		      SbBool setRegion, const SbViewportRegion &vpReg,
		      SbBool doJitter, const SbVec3f &jitterAmount)
//
////////////////////////////////////////////////////////////////////////
{
    SoState	*state = action->getState();
    SbMatrix	viewMat, projMat;

    // Compute viewing and projection matrices
    viewVol.getMatrices(viewMat, projMat);

    // Jitter if necessary
    if (doJitter) {
	SbMatrix m;
	m.setTranslate(jitterAmount);
	projMat.multRight(m);
    }

    // Set necessary elements in state
    SoProjectionMatrixElement::set(state, this, projMat);

    // Set the viewport region only if it has changed
    if (setRegion)
	SoViewportRegionElement::set(state, vpReg);

    // If the current model matrix is not identity, the camera has to
    // be transformed by it. This is equivalent to transforming the
    // world by the inverse of the model matrix. So we set the viewing
    // matrix to be the product of the inverse of the model matrix and
    // the viewMat.
    SbBool modelIsIdent;
    const SbMatrix &modelMat = SoModelMatrixElement::get(state, modelIsIdent);
    if (! modelIsIdent) {
	viewMat.multRight(modelMat.inverse());

	// Also, transform the view volume by the model matrix
	viewVol.transform(modelMat);
    }

    SoViewVolumeElement::set(state, this, viewVol);
    SoViewingMatrixElement::set(state, this, viewMat);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws cropping frame for use when rendering.
//
// Use: private

void
SoCamera::drawFrame(SoGLRenderAction *action,
		    const SbViewportRegion &vpReg,
		    const SbViewportRegion &croppedReg)
//
////////////////////////////////////////////////////////////////////////
{
// Frame color as RGB. In color index mode, just use the background
// color, since we have no idea what's in the color map.
#define GREY1  		.38, .38, .38
#define GREY1_INDEX	0

    // Get origin and size of viewports in pixels
    const SbVec2s &	vpOrig =      vpReg.getViewportOriginPixels();
    const SbVec2s &	vpSize =      vpReg.getViewportSizePixels();
    const SbVec2s &	cropLL = croppedReg.getViewportOriginPixels();
    const SbVec2s &	cropUR = croppedReg.getViewportSizePixels() + cropLL;

    // Save stuff we're going to change explicitly
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT |
		 GL_POLYGON_BIT | GL_LINE_BIT);

    glDisable(GL_LIGHTING);
    if (SoGLLazyElement::isColorIndex(action->getState()))
	glIndexi(GREY1_INDEX);
    else
	glColor3f(GREY1);

    // OK to blow away the old projection matrix, because it will be 
    // changed anyway by the camera's render action right after this...
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, vpSize[0] - 1.0, 0.0, vpSize[1] - 1.0, -1.0, 1.0);

    // Ditto for the viewing matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (viewportMapping.getValue() == CROP_VIEWPORT_LINE_FRAME)  {
	// Draw a rectangle one pixel bigger than the camera's viewport
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(1.0);
	glDisable(GL_LINE_STIPPLE);
	glRects(cropLL[0] - 1 - vpOrig[0], cropLL[1] - 1 - vpOrig[1],
		cropUR[0] + 1 - vpOrig[0], cropUR[1] + 1 - vpOrig[1]);
    }
    else if (viewportMapping.getValue() == CROP_VIEWPORT_FILL_FRAME) {
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_POLYGON_STIPPLE);

	if (croppedReg.getViewportAspectRatio() >
	    vpReg.getViewportAspectRatio()) {

	    // Draw filled rectangles for a border
	    glRects(0, 0,
		    vpSize[0] - 1, cropLL[1] - vpOrig[1] - 1);
	    glRects(0, cropUR[1] - vpOrig[1] + 1 ,
		    vpSize[0] - 1, vpSize[1] - 1);
	}
	else {
	    glRects(0, 0,
		    cropLL[0] - vpOrig[0] - 1, vpSize[1] - 1);
	    glRects(cropUR[0] - vpOrig[0] + 1, 0,
		    vpSize[0] - 1, vpSize[1] - 1);
	}
    }

    glPopMatrix();
    glPopAttrib();

#undef GREY1
#undef GREY1_INDEX
}
