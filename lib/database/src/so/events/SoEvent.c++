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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoEvent
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/events/SoEvent.h>

SoType SoEvent::classTypeId;

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoEvent::SoEvent()
//
////////////////////////////////////////////////////////////////////////
{
    timestamp	= SbTime(0, 0);
    position	= SbVec2s(0, 0);
    shiftDown	= FALSE;
    ctrlDown	= FALSE;
    altDown	= FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoEvent::~SoEvent()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// returns TRUE if event is of given type of is derived from it
//
// public
//
SbBool
SoEvent::isOfType(SoType type) const
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().isDerivedFrom(type);
}

////////////////////////////////////////////////////////////////////////
//
// returns the type id for this event
//
// virtual public
//
SoType
SoEvent::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// calculates the window position within the space of the viewport
// given by vpRgn.
//
const SbVec2s &
SoEvent::getPosition(const SbViewportRegion &vpRgn) const
//
////////////////////////////////////////////////////////////////////////
{
    SoEvent		*ev = (SoEvent *)this;
    const SbVec2s	&pixSize = vpRgn.getViewportOriginPixels();

    ev->viewportPos[0] = position[0] - pixSize[0];
    ev->viewportPos[1] = position[1] - pixSize[1];

    return viewportPos;
}

////////////////////////////////////////////////////////////////////////
//
// calculates the window position, normalizing coords to 0.0-1.0
// within the space of the viewport given by vpRgn.
//
const SbVec2f &
SoEvent::getNormalizedPosition(const SbViewportRegion &vpRgn) const
//
////////////////////////////////////////////////////////////////////////
{
    SoEvent		*ev = (SoEvent *)this;
    const SbVec2s	&pixSize = vpRgn.getViewportSizePixels();

    SbVec2f vpSize((float) pixSize[0], (float) pixSize[1]);

    SbVec2s vpPos = getPosition( vpRgn );

    if ( vpSize[0] == 0.0 )
	ev->normalizedPos[0] = 0.0;
    else
	ev->normalizedPos[0] = ((float) vpPos[0]) / vpSize[0];

    if ( vpSize[1] == 0.0 )
	ev->normalizedPos[1] = 0.0;
    else
	ev->normalizedPos[1] = ((float) vpPos[1]) / vpSize[1];

    return normalizedPos;
}
