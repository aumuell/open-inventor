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
 |	SoPickRayElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoPickRayElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoPickRayElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoPickRayElement::~SoPickRayElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoPickRayElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Nothing to initialize. Just wait for a real picking ray to be set?
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to always return FALSE. This element
//    should never have anything to do with cache invalidation.
//
// Use: public

SbBool
SoPickRayElement::matches(const SoElement *) const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoPickRayElement::matches",
		       "This method should never be called!");

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Override method to return NULL; it should never be called.
//
// Use: protected

SoElement *
SoPickRayElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoPickRayElement::copyMatchInfo",
		       "This method should never be called!");

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the ray view volume in element accessed from state.
//
// Use: public, static

void
SoPickRayElement::set(SoState *state, const SbViewVolume &volume)
//
////////////////////////////////////////////////////////////////////////
{
    SoPickRayElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoPickRayElement *) getElement(state, classStackIndex);

    if (elt != NULL)
	elt->volume = volume;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the ray view volume from the state
//
// Use: public, static

const SbViewVolume &
SoPickRayElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoPickRayElement *elt;

    elt = (const SoPickRayElement *) getConstElement(state, classStackIndex);

    return elt->volume;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints contents of element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoPickRayElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tPicking Ray\n");

    const SbVec3f	&projPt  = volume.getProjectionPoint();
    const SbVec3f	&projDir = volume.getProjectionDirection();

    fprintf(fp, "\t\tStarting point = (%g, %g, %g)\n",
	    projPt[0], projPt[1], projPt[2]);

    fprintf(fp, "\t\tDirection      = (%g, %g, %g)\n",
	    projDir[0], projDir[1], projDir[2]);

    fprintf(fp, "\t\tProjection is %s\n",
	    (volume.getProjectionType() == SbViewVolume::ORTHOGRAPHIC ?
	     "ORTHOGRAPHIC" : "PERSPECTIVE"));

    fprintf(fp, "\t\tNear distance  = %g\n", volume.getNearDist());
    fprintf(fp, "\t\tFar  distance  = %g\n",
	    volume.getNearDist() + volume.getDepth());

    fprintf(fp, "\t\tPick radius    = %g\n", 0.5 * volume.getWidth());
}
#else  /* DEBUG */
void
SoPickRayElement::print(FILE *) const
{
}
#endif /* DEBUG */
