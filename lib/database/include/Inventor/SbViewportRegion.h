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
 |	This file contains the SbViewportRegion class definition.
 |
 |   Author(s)	: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SB_VIEWPORT_REGION_
#define  _SB_VIEWPORT_REGION_

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbViewportRegion
//
//  Class that represents the active viewport region in a display
//  window. It contains the screen-space size of the window as well as
//  the origin and size of the viewport within the window. By default,
//  the viewport is the same as the full window. Methods allow the
//  viewport to be set either in terms of screen space pixels or as
//  normalized coordinates, where (0,0) is the lower-left corner of
//  the window and (1,1) is the upper-right corner.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbLinear.h>

// C-api: prefix=SbVPReg
class SbViewportRegion {
  public:
    // Default constructor. Sets window size to default 100x100
    SbViewportRegion();

    // Constructor that takes window size as width and height in pixels
    // C-api: name=CreateWH
    SbViewportRegion(short width, short height);

    // Constructor that takes window size as width and height in
    // pixels as SbVec2s
    // C-api: name=CreateVec
    SbViewportRegion(SbVec2s winSize);

    // Constructor that takes an SbViewportRegion to copy from
    // C-api: name=CreateReg
    SbViewportRegion(const SbViewportRegion &vpReg);

    // Changes window size to given width and height in pixels
    // C-api: name=SetWin
    void		setWindowSize(short width, short height)
	{ setWindowSize(SbVec2s(width, height)); }

    // Changes window size to given width and height in pixels, given
    // as SbVec2s
    // C-api: name=SetWinVec
    void		setWindowSize(SbVec2s winSize);

    // Sets viewport to given region, specified as normalized window
    // coordinates: (0,0) is the lower-left corner, (1,1) is the upper-right.
    // C-api: name=SetVP
    void		setViewport(float left, float bottom,
				    float width, float height)
	{ setViewport(SbVec2f(left, bottom), SbVec2f(width, height)); }

    // Sets viewport to region with given origin (lower-left corner)
    // and size, given as normalized coordinate vectors.
    // C-api: name=SetVPVec
    void		setViewport(SbVec2f origin, SbVec2f size);

    // Sets viewport to given region, specified as pixel coordinates
    // in window: (0,0) is the lower-left corner
    // C-api: name=SetVPPix
    void		setViewportPixels(short left, short bottom,
					  short width, short height)
	{ setViewportPixels(SbVec2s(left, bottom), SbVec2s(width, height)); }

    // Sets viewport to region with given origin (lower-left corner)
    // and size, given as pixel coordinates.
    // C-api: name=SetVPPixVec
    void		setViewportPixels(SbVec2s origin, SbVec2s size);

    // Returns window size in pixels
    // C-api: name=getWinSize
    const SbVec2s &	getWindowSize() const		{ return windowSize; }

    // Returns viewport origin in normalized coordinates
    // C-api: name=getVPOrigin
    const SbVec2f &	getViewportOrigin() const	{ return vpOrigin; }

    // Returns viewport origin in pixels
    // C-api: name=getVPOriginPix
    const SbVec2s &	getViewportOriginPixels() const	{ return vpOriginPix; }

    // Returns viewport size in normalized coordinates
    // C-api: name=getVPSize
    const SbVec2f &	getViewportSize() const		{ return vpSize; }

    // Returns viewport size in pixels
    // C-api: name=getVPSizePix
    const SbVec2s &	getViewportSizePixels() const	{ return vpSizePix;}

    // Returns aspect ratio (width/height) of viewport
    // C-api: name=getVPAspect
    float		getViewportAspectRatio() const
	{ return (vpSizePix[1] == 0 ? 1.0 :
		  (float) vpSizePix[0] / (float) vpSizePix[1]); }

    // Scales viewport within window to be the given ratio of its
    // current width or height, leaving the resulting viewport
    // centered about the same point as the current one.
    void		scaleWidth(float ratio);
    void		scaleHeight(float ratio);

    // Sets/returns the pixel-per-inch ratio for the display device
    // the viewport is part of. The default value is 72 (1 pixel per
    // printer's point).
    // C-api: name=setPixPerInch
    void		setPixelsPerInch(float ppi)   { pixelsPerInch = ppi;  }
    // C-api: name=getPixPerInch
    float		getPixelsPerInch() const      { return pixelsPerInch; }

    // Convenience function that returns number of pixels per printer's point
    // C-api: name=getPixPerPt
    float		getPixelsPerPoint() const
	{ return pixelsPerInch / 72.0; }

    // Equality comparison operator
    friend int		operator ==(const SbViewportRegion &reg1,
				    const SbViewportRegion &reg2);

  private:
    SbVec2s		windowSize;	// Window size in pixels
    SbVec2f		vpOrigin;	// Viewport origin, normalized
    SbVec2s		vpOriginPix;	// Viewport origin in pixels
    SbVec2f		vpSize;		// Viewport size, normalized
    SbVec2s		vpSizePix;	// Viewport size in pixels
    SbBool		vpSet;		// TRUE if viewport was set explicitly
    float		pixelsPerInch;	// Number of pixels per inch

    // Sets viewport to be full window
    void		setFullViewport();

    // Adjusts viewport pixel size based on new window size or new viewport
    void		adjustViewport();
};

#endif /* _SB_VIEWPORT_REGION_ */
