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
 |	SoLightAttenuationElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoLightAttenuationElement.h>

SO_ELEMENT_SOURCE(SoLightAttenuationElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoLightAttenuationElement::~SoLightAttenuationElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoLightAttenuationElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    attenuation = getDefault();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets light attenuation to given value in element accessed from state.
//
// Use: public, static

void
SoLightAttenuationElement::set(SoState *state, SoNode *node,
			       const SbVec3f &attenuation)
//
////////////////////////////////////////////////////////////////////////
{
    SoLightAttenuationElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLightAttenuationElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL)
	elt->attenuation = attenuation;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns light attenuation from state.
//
// Use: public, static

const SbVec3f &
SoLightAttenuationElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoLightAttenuationElement *elt;

    elt = (const SoLightAttenuationElement *)
	getConstElement(state, classStackIndex);

    return elt->attenuation;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to compare attenuation values.
//
// Use: public

SbBool
SoLightAttenuationElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    return (attenuation ==
	    ((const SoLightAttenuationElement *) elt)->attenuation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoLightAttenuationElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoLightAttenuationElement *result =
	(SoLightAttenuationElement *)getTypeId().createInstance();

    result->attenuation = attenuation;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoLightAttenuationElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tLight Attenuation = (%g, %g, %g)\n",
	    attenuation[0], attenuation[1], attenuation[2]);
}
#else  /* DEBUG */
void
SoLightAttenuationElement::print(FILE *) const
{
}
#endif /* DEBUG */
