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
 |   $Revision $
 |
 |   Classes:
 |	SoFontNameElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoFontNameElement.h>

SO_ELEMENT_SOURCE(SoFontNameElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoFontNameElement::~SoFontNameElement()
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
SoFontNameElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    fontName = getDefault();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the current fontName
//
// Use: public

void
SoFontNameElement::set(SoState *state, SoNode *node,
			 const SbName &fontName)
//
////////////////////////////////////////////////////////////////////////
{
    SoFontNameElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoFontNameElement *) getElement(state, classStackIndex, node);

    if (elt != NULL)
	elt->fontName = fontName;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns font name from state
//
// Use: public

const SbName &
SoFontNameElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoFontNameElement *elt;

    elt = (const SoFontNameElement *)
	getConstElement(state, classStackIndex);

    return elt->fontName;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Compares names
//
// Use: public

SbBool
SoFontNameElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    return (fontName ==
	    ((const SoFontNameElement *) elt)->fontName);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoFontNameElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoFontNameElement *result =
	(SoFontNameElement *)getTypeId().createInstance();

    result->fontName = fontName;

    return result;
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
SoFontNameElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tFontName = %s\n", fontName.getString());
}
#else  /* DEBUG */
void
SoFontNameElement::print(FILE *) const
{
}
#endif /* DEBUG */
