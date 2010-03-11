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
 |	SoFloatElement
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoFloatElement.h>

SO_ELEMENT_ABSTRACT_SOURCE(SoFloatElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoFloatElement::~SoFloatElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to return TRUE if the two
//    elements match.
//
// Use: public

SbBool
SoFloatElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    return (data == ((const SoFloatElement *) elt)->data);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set element's value.
//
// Use: public, static

void
SoFloatElement::set(int stackIndex, SoState *state, float value)
//
////////////////////////////////////////////////////////////////////////
{
    SoFloatElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoFloatElement *) getElement(state, stackIndex);

    if (elt != NULL)
	elt->setElt(value);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Really set the element.
//
// Use: protected, virtual

void
SoFloatElement::setElt(float value)
//
////////////////////////////////////////////////////////////////////////
{
    data = value;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoFloatElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoFloatElement *result =
	(SoFloatElement *)getTypeId().createInstance();

    result->data = data;

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
SoFloatElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "%s = %g\n", getTypeId().getName().getString(),
	    data);
}
#else  /* DEBUG */
void
SoFloatElement::print(FILE *) const
{
}
#endif /* DEBUG */
