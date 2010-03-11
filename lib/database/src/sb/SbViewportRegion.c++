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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbViewportRegion
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbViewportRegion.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor. Sets window size to default 100x100.
//
// Use: public

SbViewportRegion::SbViewportRegion()
//
////////////////////////////////////////////////////////////////////////
{
    windowSize.setValue(100, 100);

    // Viewport is full window
    setFullViewport();

    vpSet = FALSE;

    pixelsPerInch = 72.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes window size as width and height in pixels.
//
// Use: public

SbViewportRegion::SbViewportRegion(short width, short height)
//
////////////////////////////////////////////////////////////////////////
{
    windowSize.setValue(width, height);

    // Viewport is full window
    setFullViewport();

    vpSet = FALSE;

    pixelsPerInch = 72.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes window size as width and height in pixels
//    as SbVec2s.
//
// Use: public

SbViewportRegion::SbViewportRegion(SbVec2s winSize)
//
////////////////////////////////////////////////////////////////////////
{
    windowSize = winSize;

    // Viewport is full window
    setFullViewport();

    vpSet = FALSE;

    pixelsPerInch = 72.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes an SbViewportRegion to copy from.
//
// Use: public

SbViewportRegion::SbViewportRegion(const SbViewportRegion &vpReg)
//
////////////////////////////////////////////////////////////////////////
{
    *this = vpReg;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes window size to given width and height in pixels.
//
// Use: public

void
SbViewportRegion::setWindowSize(SbVec2s winSize)
//
////////////////////////////////////////////////////////////////////////
{
    windowSize = winSize;

    if (! vpSet)
	setFullViewport();

    else
	adjustViewport();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets viewport to region with given origin (lower-left corner)
//    and size, given as normalized coordinate vectors.
//
// Use: public

void
SbViewportRegion::setViewport(SbVec2f origin, SbVec2f size)
//
////////////////////////////////////////////////////////////////////////
{
    vpOrigin = origin;
    vpSize   = size;

    adjustViewport();

    vpSet = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets viewport to region with given origin (lower-left corner)
//    and size, given as pixel coordinates.
//
// Use: public

void
SbViewportRegion::setViewportPixels(SbVec2s origin, SbVec2s size)
//
////////////////////////////////////////////////////////////////////////
{
    vpOriginPix = origin;
    vpSizePix   = size;

    // Compute normalized coordinates for viewport origin and size
    vpOrigin.setValue((float) vpOriginPix[0] / (float) windowSize[0],
		      (float) vpOriginPix[1] / (float) windowSize[1]);
    vpSize.setValue((float) vpSizePix[0] / (float) windowSize[0],
		    (float) vpSizePix[1] / (float) windowSize[1]);

    vpSet = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales viewport within window to be the given ratio of its
//    current width, leaving the resulting viewport centered about the
//    same point as the current one.
//
// Use: public

void
SbViewportRegion::scaleWidth(float ratio)
//
////////////////////////////////////////////////////////////////////////
{
    float	halfWidth   = vpSize[0] / 2.0;
    float	widthCenter = vpOrigin[0] + halfWidth;

    vpOrigin[0] = widthCenter - ratio * halfWidth;
    vpSize[0]  *= ratio;

    // Make sure the viewport remains in the window
    if (vpOrigin[0] < 0.0)
	vpOrigin[0] = 0.0;
    if (vpSize[0] > 1.0)
	vpSize[0] = 1.0;

    adjustViewport();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales viewport within window to be the given ratio of its
//    current height, leaving the resulting viewport centered about the
//    same point as the current one.
//
// Use: public

void
SbViewportRegion::scaleHeight(float ratio)
//
////////////////////////////////////////////////////////////////////////
{
    float	halfHeight   = vpSize[1] / 2.0;
    float	heightCenter = vpOrigin[1] + halfHeight;

    vpOrigin[1] = heightCenter - ratio * halfHeight;
    vpSize[1]  *= ratio;

    // Make sure the viewport remains in the window
    if (vpOrigin[1] < 0.0)
	vpOrigin[1] = 0.0;
    if (vpSize[1] > 1.0)
	vpSize[1] = 1.0;

    adjustViewport();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if two viewport regions are identical.
//
// Use: public

int
operator ==(const SbViewportRegion &reg1, const SbViewportRegion &reg2)
{
    return (reg1.windowSize == reg2.windowSize &&
	    reg1.vpOrigin   == reg2.vpOrigin   &&
	    reg1.vpSize     == reg2.vpSize);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets viewport to full window. Assumes viewport was not set
//    explicitly by caller.
//
// Use: private

void
SbViewportRegion::setFullViewport()
//
////////////////////////////////////////////////////////////////////////
{
    vpOrigin.setValue(0.0, 0.0);
    vpSize.setValue(1.0, 1.0);
    vpOriginPix.setValue(0, 0);
    vpSizePix = windowSize;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adjusts viewport pixel size based on new window size or new
//    viewport. Assumes viewport was set explicitly by caller.
//
// Use: private

void
SbViewportRegion::adjustViewport()
//
////////////////////////////////////////////////////////////////////////
{
    vpOriginPix.setValue((short) (vpOrigin[0] * windowSize[0]),
			 (short) (vpOrigin[1] * windowSize[1]));
    vpSizePix.setValue((short) (vpSize[0] * windowSize[0]),
		       (short) (vpSize[1] * windowSize[1]));
}
